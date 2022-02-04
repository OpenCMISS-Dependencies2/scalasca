/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "ReportWriterTask.h"

#include <sys/stat.h>

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <vector>

#include <pearl/Callpath.h>
#include <pearl/Callsite.h>
#include <pearl/CartTopology.h>
#include <pearl/CommSet.h>
#include <pearl/EnterProgram_rep.h>
#include <pearl/Error.h>
#include <pearl/GlobalDefs.h>
#include <pearl/LocalTrace.h>
#include <pearl/LocationGroup.h>
#include <pearl/Region.h>
#include <pearl/String.h>
#include <pearl/SystemNode.h>
#include <pearl/Utils.h>

#include <UTILS_Error.h>

#include "AnalyzeTask.h"
#include "Logging.h"
#include "ReportData.h"
#include "TaskData.h"

#if defined(_MPI)
    #include <mpi.h>
#endif    // _MPI

using namespace std;
using namespace pearl;
using namespace scout;


// --- Symbolic names -------------------------------------------------------

#define OUTPUT_FILENAME    "scout"
#define PRIMARY_MIRROR                                      \
    "http://apps.fz-juelich.de/scalasca/releases/scalasca/" \
    SCALASCA_VERSION "/help/"
#define SECONDARY_MIRROR                                           \
    "http://perftools.pages.jsc.fz-juelich.de/cicd/scalasca/tags/" \
    "scalasca-" PACKAGE_VERSION "/patterns/"
#define TERTIARY_MIRROR                                            \
    "http://perftools.pages.jsc.fz-juelich.de/cicd/scalasca/tags/" \
    "latest/patterns/"


// --- Constructors & destructor --------------------------------------------

/**
 *  @brief Constructor. Creates a new report writer task.
 *
 *  This constructor creates a new report writer task instance using the
 *  given parameters.
 *
 *  @param  sharedData   Shared task data object
 *  @param  privateData  Private task data object
 *  @param  analyzer     Analyzer task to query for the severity data
 *  @param  dirname      Experiment archive directory name
 *  @param  rank         Global process rank (MPI rank or 0)
 **/
ReportWriterTask::ReportWriterTask(const TaskDataShared&  sharedData,
                                   const TaskDataPrivate& privateData,
                                   const AnalyzeTask*     analyzer,
                                   const string&          dirname,
                                   int                    rank)
    : Task(),
      mSharedData(sharedData),
      mPrivateData(privateData),
      mAnalyzer(analyzer),
      mDirname(dirname),
      mRank(rank)
{
}


// --- Executing the task ---------------------------------------------------

/**
 *  @brief Executes the report writer task.
 *
 *  The writer task first creates the CUBE output file, writes the static
 *  definition data (meta data) to it, and incrementally collates and writes
 *  the per-pattern severity data.
 *
 *  @return Returns @em true if the execution was successful, @em false
 *          otherwise
 *  @throw  pearl::FatalError  if the CUBE file could not be created.
 **/
bool
ReportWriterTask::execute()
{
    // Keep track of number of program arguments (if available).  Since the
    // ENTER_PROGRAM event is only stored on the master location, no data race
    // is possible here.
    static uint32_t maxProgramArgs = 0;
    Event           firstEvt       = mPrivateData.mTrace->begin();
    if (  firstEvt.is_valid()
       && firstEvt->isOfType(ENTER_PROGRAM))
    {
        EnterProgram_rep& enter = event_cast< EnterProgram_rep >(*firstEvt);

        maxProgramArgs = enter.numArguments();
    }

    // This variable is implicitly shared!
    static int error = 0;

    ReportData data(*mSharedData.mDefinitions, mRank);
    if (mRank == 0)
    {
        #pragma omp master
        {
            // Create CUBE object & file
            std::string filename = mDirname + "/" + OUTPUT_FILENAME;
            data.cb = cube_create(const_cast< char* >(filename.c_str()), CUBE_MASTER, CUBE_FALSE);

            // Broadcast whether CUBE file could be opened
            error = (NULL == data.cb) ? 1 : 0;
            #if defined(_MPI)
                MPI_Bcast(&error, 1, MPI_INT, 0, MPI_COMM_WORLD);
                MPI_Allreduce(MPI_IN_PLACE, &maxProgramArgs,
                              1, SCALASCA_MPI_UINT32_T, MPI_MAX, MPI_COMM_WORLD);
            #endif    // _MPI
        }
        #pragma omp barrier
        if (error)
        {
            throw FatalError("Could not create CUBE report!");
        }
        #pragma omp master
        {
            // Write static definition data
            cube_def_mirror(data.cb, PRIMARY_MIRROR);
            cube_def_mirror(data.cb, SECONDARY_MIRROR);
            cube_def_mirror(data.cb, TERTIARY_MIRROR);
            cube_def_attr(data.cb,
                          "Creator",
                          "Scalasca Trace Analyzer " PACKAGE_VERSION);
            cube_def_attr(data.cb, "CUBE_CT_AGGR", "SUM");
            cube_def_attr(data.cb,
                          "Score-P::DefinitionArguments::ProgramBegin::numberOfArguments",
                          toStdString(maxProgramArgs).c_str());

            embed_specfile(data);
            gen_sysinfo(data);
            gen_regions(data);
            gen_cnodes(data);
            gen_cartesians(data);
            mAnalyzer->genCubeMetrics(data);
        }

        // Collate severity data
        mAnalyzer->genCubeSeverities(data, mRank);

        // Clean up
        #pragma omp master
        {
            cube_free(data.cb);
        }
    }
    else
    {
        #pragma omp master
        {
            // Check whether CUBE file could be opened
            #if defined(_MPI)
                MPI_Bcast(&error, 1, MPI_INT, 0, MPI_COMM_WORLD);
                MPI_Allreduce(MPI_IN_PLACE, &maxProgramArgs,
                              1, SCALASCA_MPI_UINT32_T, MPI_MAX, MPI_COMM_WORLD);
            #endif    // _MPI
        }
        #pragma omp barrier
        if (error)
        {
            throw FatalError("Could not create CUBE report!");
        }

        // Collate severity data
        mAnalyzer->genCubeSeverities(data, mRank);
    }

    return true;
}


// --- Report data generation -----------------------------------------------

/**
 *  @brief Embeds the remapping specification file.
 *
 *  Embeds the remapping specification file from the installation into the
 *  CUBE report represented by the CUBE object passed via @a data.
 *
 *  @param data
 *      Report data structure
 **/
void
ReportWriterTask::embed_specfile(ReportData& data) const
{
    const char* const fileName = PKGDATADIR "/scout.spec";
    const char* const message  = "Not embedding spec file in Cube report!";

    // Check whether file exists and get its size
    struct stat statBuffer;
    if (stat(fileName, &statBuffer) != 0)
    {
        UTILS_WARNING("%s: %s\n%s", fileName, strerror(errno), message);

        return;
    }
    const size_t fileSize = statBuffer.st_size;

    // Create memory buffer
    vector< char > buffer(fileSize);

    // Read spec file into buffer
    FILE* fp = fopen(fileName, "rb");
    if (!fp)
    {
        UTILS_WARNING("%s: %s\n%s", fileName, strerror(errno), message);

        return;
    }
    if (fread(&buffer.front(), fileSize, 1, fp) != 1)
    {
        UTILS_WARNING("%s: Error reading file\n%s", fileName, message);
        fclose(fp);

        return;
    }
    fclose(fp);

    // Write buffer to Cube report
    cube_write_misc_data(data.cb, "remapping.spec", &buffer.front(), fileSize);
}


/**
 *  @brief Generates the system hierarchy information.
 *
 *  This method defines the CUBE system hierarchy in the CUBE object passed
 *  via @a data based on the information available in the global definitions
 *  object.
 *
 *  @param  data  Report data structure
 *
 *  @todo
 *    - Fix implicit assumption that location group ID == MPI rank
 **/
void
ReportWriterTask::gen_sysinfo(ReportData& data) const
{
    const GlobalDefs& defs = *mSharedData.mDefinitions;

    // Temporary mapping table: system tree node ID |-> CUBE system tree node
    std::vector< cube_system_tree_node* > snodes;

    // Temporary mapping table: location group ID |-> CUBE location group
    std::vector< cube_location_group* > lgroups;

    // Create system tree nodes
    const size_t numSystemNodes = defs.numSystemNodes();
    for (size_t index = 0; index < numSystemNodes; ++index)
    {
        const SystemNode& snode = defs.getSystemNodeByIndex(index);

        // Determine parent
        SystemNode*            parent  = snode.getParent();
        cube_system_tree_node* cparent = NULL;
        if (parent)
        {
            cparent = snodes[parent->getId()];
        }

        // Construct node name
        string name = snode.getClassName().getString() + " "
                      + snode.getName().getString();

        // Create node
        cube_system_tree_node* csnode;
        csnode = cube_def_system_tree_node(data.cb,
                                           name.c_str(),
                                           "",
                                           snode.getClassName().getCString(),
                                           cparent);

        snodes.push_back(csnode);
    }

    // Create location groups
    const size_t numLocationGroups = defs.numLocationGroups();
    for (size_t index = 0; index < numLocationGroups; ++index)
    {
        const LocationGroup& lgroup = defs.getLocationGroupByIndex(index);

        // Convert location group type
        cube_location_group_type ctype;
        ostringstream            lgname;
        switch (lgroup.getType())
        {
            case LocationGroup::TYPE_PROCESS:
                ctype = CUBE_LOCATION_GROUP_TYPE_PROCESS;
                lgname << "MPI rank " << lgroup.getId();
                break;

            default:
                throw RuntimeError("Unknown location group type!");
        }

        // Create location group
        cube_location_group* clgroup;
        clgroup = cube_def_location_group(data.cb,
                                          (lgroup.getName().getString().empty()
                                           ? lgname.str().c_str()
                                           : lgroup.getName().getCString()),
                                          lgroup.getId(),
                                          ctype,
                                          snodes[lgroup.getParent()->getId()]);
        lgroups.push_back(clgroup);

        // Create locations
        const uint32_t numLocations = lgroup.numLocations();
        for (uint32_t locIdx = 0; locIdx < numLocations; ++locIdx)
        {
            const Location& location = lgroup.getLocation(locIdx);

            // Convert location type
            cube_location_type ctype;
            ostringstream      lname;
            switch (location.getType())
            {
                case Location::TYPE_CPU_THREAD:
                    ctype = CUBE_LOCATION_TYPE_CPU_THREAD;
                    lname << "CPU thread " << location.getThreadId();
                    break;

                case Location::TYPE_GPU:
                    ctype = CUBE_LOCATION_TYPE_GPU;
                    lname << "GPU " << location.getThreadId();
                    break;

                case Location::TYPE_METRIC:
                    ctype = CUBE_LOCATION_TYPE_METRIC;
                    lname << "metric location " << location.getThreadId();
                    break;

                default:
                    throw RuntimeError("Unknown location type!");
            }

            // Create location
            cube_location* clocation;
            clocation = cube_def_location(data.cb,
                                          (location.getName().getString().empty()
                                           ? lname.str().c_str()
                                           : location.getName().getCString()),
                                          location.getThreadId(),
                                          ctype,
                                          lgroups[location.getParent()->getId()]);

            data.locs[location.getId()] = clocation;
        }
    }
}


/**
 *  @brief Generates the source-code region information.
 *
 *  This method defines the CUBE source-code regions in the CUBE object passed
 *  via @a data based on the information available in the global definitions
 *  object.
 *
 *  @param  data  Report data structure
 *
 *  @todo Add support for canonical names.
 **/
void
ReportWriterTask::gen_regions(ReportData& data) const
{
    const GlobalDefs& defs = *mSharedData.mDefinitions;

    // Create regions
    const size_t numRegions = defs.numRegions();
    for (size_t index = 0; index < numRegions; ++index)
    {
        const Region& region  = defs.getRegionByIndex(index);
        cube_region*  cregion = cube_def_region(data.cb,
                                                region.getDisplayName().getCString(),
                                                region.getCanonicalName().getCString(),
                                                pearl::toString(region.getParadigm()).c_str(),
                                                pearl::toString(region.getRole()).c_str(),
                                                region.getStartLine(),
                                                region.getEndLine(),
                                                "",
                                                region.getDescription().getCString(),
                                                region.getFilename().getCString());
        data.regions.push_back(cregion);
    }
}


/**
 *  @brief Generates the call tree information.
 *
 *  This method defines the CUBE call tree in the CUBE object passed via @a
 *  data based on the information available in the global definitions object.
 *
 *  @param  data  Report data structure
 **/
void
ReportWriterTask::gen_cnodes(ReportData& data) const
{
    const GlobalDefs& defs = *mSharedData.mDefinitions;

    // Create call-tree nodes
    const size_t numCallpaths = defs.numCallpaths();
    for (size_t index = 0; index < numCallpaths; ++index)
    {
        const Callpath& callpath = defs.getCallpathByIndex(index);

        // Special case: "UNKNOWN" callpath found in EPIK traces
        const Region& region = callpath.getRegion();
        if (  (region.getDisplayName().getString() == "UNKNOWN")
           && (region.getFilename().getString() == "EPIK")
           && (region.getDescription().getString() == "EPIK"))
        {
            // Create dummy entry
            data.cnodes.push_back(NULL);
            continue;
        }

        // Determine parent
        Callpath*   parent  = callpath.getParent();
        cube_cnode* cparent = NULL;
        if (parent)
        {
            cparent = data.cnodes[parent->getId()];
        }

        // Create node
        cube_cnode*     ccallpath;
        const Callsite& csite = callpath.getCallsite();
        if (csite == Callsite::UNDEFINED)
        {
            ccallpath = cube_def_cnode(data.cb,
                                       data.regions[region.getId()],
                                       cparent);
        }
        else
        {
            ccallpath = cube_def_cnode_cs(data.cb,
                                          data.regions[csite.getCallee().getId()],
                                          csite.getFilename().getCString(),
                                          csite.getLine(),
                                          cparent);
        }
        data.cnodes.push_back(ccallpath);
    }
}


/**
 *  @brief Generates the topology information.
 *
 *  This method defines the CUBE cartesian topology in the CUBE object passed
 *  via @p data based on the information available in the global definitions
 *  object.
 *
 *  @param data
 *      Report data structure
 **/
void
ReportWriterTask::gen_cartesians(ReportData& data) const
{
    const GlobalDefs& defs = *mSharedData.mDefinitions;

    const size_t numCartTopologies = defs.numCartTopologies();
    for (size_t index = 0; index < numCartTopologies; ++index)
    {
        const CartTopology& topology      = defs.getCartTopologyByIndex(index);
        const uint32_t      numDimensions = topology.numDimensions();

        // Define Cartesian grid topology
        vector< long > dimSizes;
        dimSizes.reserve(numDimensions);

        vector< int > dimPeriodicity;
        dimPeriodicity.reserve(numDimensions);

        for (uint8_t dimIndex = 0; dimIndex < numDimensions; ++dimIndex)
        {
            const CartDimension& dimension = topology.getDimension(dimIndex);

            dimSizes.push_back(dimension.getSize());
            dimPeriodicity.push_back(dimension.isPeriodic() ? 1 : 0);
        }

        cube_cartesian* ccart = cube_def_cart(data.cb,
                                              numDimensions,
                                              dimSizes.empty()
                                              ? 0
                                              : &dimSizes[0],
                                              dimPeriodicity.empty()
                                              ? 0
                                              : &dimPeriodicity[0]);

        const String& topologyName = topology.getName();
        if (topologyName != String::UNDEFINED)
        {
            cube_cart_set_name(ccart, topologyName.getCString());
        }

        for (uint8_t dimIndex = 0; dimIndex < numDimensions; ++dimIndex)
        {
            const CartDimension& dimension = topology.getDimension(dimIndex);

            const String& dimName = dimension.getName();
            if (dimName != String::UNDEFINED)
            {
                cube_cart_set_dim_name(ccart, dimName.getCString(), dimIndex);
            }
        }

        // Define coordinates
        vector< long > coordinate;
        coordinate.reserve(numDimensions);

        const Communicator& communicator = topology.getCommunicator();
        const CommSet&      commSet      = communicator.getCommSet();

        const uint32_t numRanks = communicator.getSize();
        for (uint32_t rank = 0; rank < numRanks; ++rank)
        {
            const Location&     location  = commSet.getLocation(rank);
            const CartCoordList coordList = topology.getRankCoordinates(rank);

            CartCoordList::const_iterator it = coordList.begin();
            while (it != coordList.end())
            {
                coordinate.assign(it->begin(), it->end());
                cube_def_coords(data.cb,
                                ccart,
                                data.locs[location.getId()],
                                coordinate.empty()
                                ? 0
                                : &coordinate[0]);
                coordinate.clear();

                ++it;
            }
        }
    }
}
