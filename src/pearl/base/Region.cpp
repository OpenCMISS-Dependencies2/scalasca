/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
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


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Implementation of the class Region.
 *
 *  This file provides the implementation of the class Region and related
 *  functions.
 *
 *  @todo
 *    - Revise classification functions to use region roles/paradigms.
 *    - Move lowercase() to some other, more central location
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/Region.h>

#include <algorithm>
#include <cassert>
#include <cctype>
#include <functional>
#include <iostream>

#include <pearl/String.h>

#include "iomanip_detail.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Local helper functions -----------------------------------------------

namespace
{
string
lowercase(const string& str);

uint32_t
classify(const string& name,
         const string& file,
         Region::Role  role,
         Paradigm      paradigm);

uint32_t
classify_omp(const string& name);

uint32_t
classify_mpi(const string& name);

uint32_t
classify_pthread(const string& name);
}    // unnamed namespace


// --- Constructors & destructor --------------------------------------------

Region::Region(const IdType   id,
               const String&  canonicalName,
               const String&  displayName,
               const String&  description,
               const Role     role,
               const Paradigm paradigm,
               const String&  filename,
               const uint32_t startLine,
               const uint32_t endLine)
    : mIdentifier(id),
      mCanonicalName(canonicalName),
      mDisplayName(displayName),
      mDescription(description),
      mRole(role),
      mParadigm(paradigm),
      mFilename(filename),
      mStartLine(startLine),
      mEndLine(endLine)
{
    // Arbitrary int's can be assigned to an enum, i.e., we need a range check
    assert(  (role >= Region::ROLE_UNKNOWN)
          && (role < Region::NUMBER_OF_ROLES));

    mClass = classify(canonicalName.getString(), filename.getString(),
                      role, paradigm);
}


// --- Access definition data -----------------------------------------------

Region::IdType
Region::getId() const
{
    return mIdentifier;
}


const String&
Region::getCanonicalName() const
{
    return mCanonicalName;
}


const String&
Region::getDisplayName() const
{
    return mDisplayName;
}


const String&
Region::getDescription() const
{
    return mDescription;
}


Region::Role
Region::getRole() const
{
    return mRole;
}


Paradigm
Region::getParadigm() const
{
    return mParadigm;
}


const String&
Region::getFilename() const
{
    return mFilename;
}


uint32_t
Region::getStartLine() const
{
    return mStartLine;
}


uint32_t
Region::getEndLine() const
{
    return mEndLine;
}


// --- Stream I/O operators -------------------------------------------------

ostream&
pearl::operator<<(ostream&      stream,
                  const Region& item)
{
    const long detail = getDetail(stream);

    // Special case: skipped output
    if (detail <= DETAIL_SKIP)
    {
        return stream;
    }

    // Special case: undefined region
    if (item == Region::UNDEFINED)
    {
        return stream << "<undef>";
    }

    // Special case: abbreviated output
    if (detail == DETAIL_ABBRV)
    {
        return stream << item.getDisplayName();
    }

    // Print data
    stream << "Region"
           << beginObject
           << "id: " << item.getId()
           << sep
           << "name: " << item.getDisplayName()
           << sep
           << "desc: " << item.getDescription()
           << sep
           << "role: " << toString(item.getRole())
           << sep
           << "paradigm: " << item.getParadigm()
           << sep
           << "file: " << item.getFilename()
           << sep
           << "begin: " << item.getStartLine()
           << sep
           << "end: " << item.getEndLine()
           << endObject;

    return stream;
}


// --- Comparison operators -------------------------------------------------

bool
pearl::operator==(const Region& lhs,
                  const Region& rhs)
{
    // Shortcut: compare addresses
    if (&lhs == &rhs)
    {
        return true;
    }

    return (  (lhs.getId() == rhs.getId())
           && (lhs.getCanonicalName() == rhs.getCanonicalName())
           && (lhs.getDisplayName() == rhs.getDisplayName())
           && (lhs.getDescription() == rhs.getDescription())
           && (lhs.getRole() == rhs.getRole())
           && (lhs.getParadigm() == rhs.getParadigm())
           && (lhs.getFilename() == rhs.getFilename())
           && (lhs.getStartLine() == rhs.getStartLine())
           && (lhs.getEndLine() == rhs.getEndLine()));
}


bool
pearl::operator!=(const Region& lhs,
                  const Region& rhs)
{
    return !(lhs == rhs);
}


// --- Conversion functions -------------------------------------------------

const string
pearl::toString(const Region::Role role)
{
    switch (role)
    {
        case Region::ROLE_UNKNOWN:
            return "unknown";

        case Region::ROLE_FUNCTION:
            return "function";

        case Region::ROLE_WRAPPER:
            return "wrapper";

        case Region::ROLE_LOOP:
            return "loop";

        case Region::ROLE_CODE:
            return "code";

        case Region::ROLE_PARALLEL:
            return "parallel";

        case Region::ROLE_SECTIONS:
            return "sections";

        case Region::ROLE_SECTION:
            return "section";

        case Region::ROLE_SINGLE:
            return "single";

        case Region::ROLE_SINGLE_SBLOCK:
            return "single sblock";

        case Region::ROLE_WORKSHARE:
            return "workshare";

        case Region::ROLE_MASTER:
            return "master";

        case Region::ROLE_CRITICAL:
            return "critical";

        case Region::ROLE_CRITICAL_SBLOCK:
            return "critical sblock";

        case Region::ROLE_BARRIER:
            return "barrier";

        case Region::ROLE_IMPLICIT_BARRIER:
            return "implicit barrier";

        case Region::ROLE_ATOMIC:
            return "atomic";

        case Region::ROLE_FLUSH:
            return "flush";

        case Region::ROLE_ORDERED:
            return "ordered";

        case Region::ROLE_ORDERED_SBLOCK:
            return "ordered sblock";

        case Region::ROLE_TASK:
            return "task";

        case Region::ROLE_TASK_CREATE:
            return "task create";

        case Region::ROLE_TASK_WAIT:
            return "taskwait";

        case Region::ROLE_COLL_ONE2ALL:
            return "one2all";

        case Region::ROLE_COLL_ALL2ONE:
            return "all2one";

        case Region::ROLE_COLL_ALL2ALL:
            return "all2all";

        case Region::ROLE_COLL_OTHER:
            return "other collective";

        case Region::ROLE_FILE_IO:
            return "file io";

        case Region::ROLE_POINT2POINT:
            return "point2point";

        case Region::ROLE_RMA:
            return "rma";

        case Region::ROLE_DATA_TRANSFER:
            return "data transfer";

        case Region::ROLE_ARTIFICIAL:
            return "artificial";

        case Region::ROLE_THREAD_CREATE:
            return "thread create";

        case Region::ROLE_THREAD_WAIT:
            return "thread wait";

        case Region::ROLE_TASK_UNTIED:
            return "untied task";

        case Region::ROLE_ALLOCATE:
            return "allocate";

        case Region::ROLE_DEALLOCATE:
            return "deallocate";

        case Region::ROLE_REALLOCATE:
            return "reallocate";

        // For "NUMBER_OF_ROLES" -- to make the compiler happy...
        default:
            break;
    }

    // Since all possible region roles should be handled in the switch
    // statement above, something went wrong if we reach this point...
    assert(false);

    return "";
}


// --- Local helper functions -----------------------------------------------

namespace
{
struct fo_tolower
    : public std::unary_function< int, int >
{
    int
    operator()(int x) const
    {
        return std::tolower(x);
    }
};


string
lowercase(const string& str)
{
    string result(str);

    transform(str.begin(), str.end(), result.begin(), fo_tolower());

    return result;
}


uint32_t
classify(const string& name,
         const string& file,
         Region::Role  role,
         Paradigm      paradigm)
{
    // Measurement-related regions
    if (  (paradigm == Paradigm::MEASUREMENT_SYSTEM)
       || (file == "EPIK"))
    {
        return Region::cCLASS_INTERNAL;
    }

    // MPI regions
    if (file == "MPI")
    {
        return classify_mpi(lowercase(name.substr(4)));
    }

    // OpenMP regions
    if (  (name.compare(0, 5, "!$omp") == 0)
       || (file == "OMP"))
    {
        return classify_omp(lowercase(name));
    }

    // Pthread regions
    if (file == "PTHREAD")
    {
        return classify_pthread(lowercase(name.substr(8)));
    }

    // User regions
    return Region::cCLASS_USER;
}


uint32_t
classify_pthread(const string& name)
{
    string pthreadName(name);

    if (pthreadName == "mutex_lock")
    {
        return (Region::cCLASS_PTHREAD
                | Region::cCAT_PTHREAD_SYNC
                | Region::cTYPE_PTHREAD_MUTEX
                | Region::cMODE_PTHREAD_LOCK);
    }
    if (pthreadName == "mutex_trylock")
    {
        return (Region::cCLASS_PTHREAD
                | Region::cCAT_PTHREAD_SYNC
                | Region::cTYPE_PTHREAD_MUTEX
                | Region::cMODE_PTHREAD_TRYLOCK);
    }
    if (pthreadName == "mutex_unlock")
    {
        return (Region::cCLASS_PTHREAD
                | Region::cCAT_PTHREAD_SYNC
                | Region::cTYPE_PTHREAD_MUTEX
                | Region::cMODE_PTHREAD_UNLOCK);
    }
    if (pthreadName == "spin_lock")
    {
        return (Region::cCLASS_PTHREAD
                | Region::cCAT_PTHREAD_SYNC
                | Region::cTYPE_PTHREAD_SPIN
                | Region::cMODE_PTHREAD_LOCK);
    }
    if (pthreadName == "spin_trylock")
    {
        return (Region::cCLASS_PTHREAD
                | Region::cCAT_PTHREAD_SYNC
                | Region::cTYPE_PTHREAD_SPIN
                | Region::cMODE_PTHREAD_TRYLOCK);
    }
    if (pthreadName == "spin_unlock")
    {
        return (Region::cCLASS_PTHREAD
                | Region::cCAT_PTHREAD_SYNC
                | Region::cTYPE_PTHREAD_SPIN
                | Region::cMODE_PTHREAD_UNLOCK);
    }
    if (pthreadName == "cond_timedwait")
    {
        return (Region::cCLASS_PTHREAD
                | Region::cCAT_PTHREAD_SYNC
                | Region::cTYPE_PTHREAD_CONDITION
                | Region::cMODE_PTHREAD_TIMEDWAIT);
    }
    if (pthreadName == "cond_wait")
    {
        return (Region::cCLASS_PTHREAD
                | Region::cCAT_PTHREAD_SYNC
                | Region::cTYPE_PTHREAD_CONDITION
                | Region::cMODE_PTHREAD_WAIT);
    }

    return Region::cCLASS_PTHREAD;
}


uint32_t
classify_omp(const string& name)
{
    string ompName(name);

    string::size_type pos = name.find('@');
    if (pos != string::npos)
    {
        ompName.erase(pos - 1);
    }
    if (ompName == "!$omp parallel")
    {
        return (Region::cCLASS_OMP
                | Region::cCAT_OMP_PARALLEL);
    }
    if (ompName == "!$omp barrier")
    {
        return (Region::cCLASS_OMP
                | Region::cCAT_OMP_SYNC
                | Region::cTYPE_OMP_BARRIER
                | Region::cMODE_OMP_EXPLICIT);
    }
    if (  (ompName == "!$omp ibarrier")
       || (ompName == "!$omp implicit barrier"))
    {
        return (Region::cCLASS_OMP
                | Region::cCAT_OMP_SYNC
                | Region::cTYPE_OMP_BARRIER
                | Region::cMODE_OMP_IMPLICIT);
    }

    return Region::cCLASS_OMP;
}


uint32_t
classify_mpi(const string& name)
{
    // MPI setup
    if (  (name == "init")
       || (name == "init_thread"))
    {
        return (Region::cCLASS_MPI
                | Region::cCAT_MPI_SETUP
                | Region::cTYPE_MPI_INIT);
    }
    if (name == "finalize")
    {
        return (Region::cCLASS_MPI
                | Region::cCAT_MPI_SETUP
                | Region::cTYPE_MPI_FINALIZE);
    }

    // MPI collective
    if (name == "barrier")
    {
        return (Region::cCLASS_MPI
                | Region::cCAT_MPI_COLLECTIVE
                | Region::cTYPE_MPI_BARRIER);
    }
    if (  (name == "bcast")
       || (name == "scatter")
       || (name == "scatterv"))
    {
        return (Region::cCLASS_MPI
                | Region::cCAT_MPI_COLLECTIVE
                | Region::cTYPE_MPI_ONE_TO_N);
    }
    if (  (name == "reduce")
       || (name == "gather")
       || (name == "gatherv"))
    {
        return (Region::cCLASS_MPI
                | Region::cCAT_MPI_COLLECTIVE
                | Region::cTYPE_MPI_N_TO_ONE);
    }
    if (  (name == "allgather")
       || (name == "allgatherv")
       || (name == "allreduce")
       || (name == "alltoall")
       || (name == "alltoallv")
       || (name == "alltoallw")
       || (name == "reduce_scatter")
       || (name == "reduce_scatter_block"))
    {
        return (Region::cCLASS_MPI
                | Region::cCAT_MPI_COLLECTIVE
                | Region::cTYPE_MPI_N_TO_N);
    }
    if (  (name == "scan")
       || (name == "exscan"))
    {
        return (Region::cCLASS_MPI
                | Region::cCAT_MPI_COLLECTIVE
                | Region::cTYPE_MPI_PARTIAL);
    }

    // MPI point-to-point
    if (name.compare(0, 8, "sendrecv") == 0)
    {
        return (Region::cCLASS_MPI
                | Region::cCAT_MPI_P2P
                | Region::cTYPE_MPI_SENDRECV
                | Region::cMODE_MPI_NONBLOCKING);
    }
    if (name.find("send") != string::npos)
    {
        uint32_t result = Region::cCLASS_MPI | Region::cCAT_MPI_P2P | Region::cTYPE_MPI_SEND;

        if (name[0] == 'i')
        {
            result |= Region::cMODE_MPI_NONBLOCKING;
        }
        else if (name.find("init") != string::npos)
        {
            result |= Region::cMODE_MPI_PERSISTENT;
        }
        if (name.find("ssend") != string::npos)
        {
            result |= Region::cMODE_MPI_SYNCHRONOUS;
        }
        else if (name.find("bsend") != string::npos)
        {
            result |= Region::cMODE_MPI_BUFFERED;
        }
        else if (name.find("rsend") != string::npos)
        {
            result |= Region::cMODE_MPI_READY;
        }
        else
        {
            result |= Region::cMODE_MPI_STANDARD;
        }

        return result;
    }

    if (name == "recv")
    {
        return (Region::cCLASS_MPI
                | Region::cCAT_MPI_P2P
                | Region::cTYPE_MPI_RECV);
    }
    if (name == "irecv")
    {
        return (Region::cCLASS_MPI
                | Region::cCAT_MPI_P2P
                | Region::cTYPE_MPI_RECV
                | Region::cMODE_MPI_NONBLOCKING);
    }
    if (name == "recv_init")
    {
        return (Region::cCLASS_MPI
                | Region::cCAT_MPI_P2P
                | Region::cTYPE_MPI_RECV
                | Region::cMODE_MPI_PERSISTENT);
    }
    if (  (name == "test")
       || (name == "testany"))
    {
        return (Region::cCLASS_MPI
                | Region::cCAT_MPI_P2P
                | Region::cTYPE_MPI_TEST
                | Region::cMODE_MPI_SINGLE);
    }
    if (  (name == "testall")
       || (name == "testsome"))
    {
        return (Region::cCLASS_MPI
                | Region::cCAT_MPI_P2P
                | Region::cTYPE_MPI_TEST
                | Region::cMODE_MPI_MANY);
    }
    if (  (name == "wait")
       || (name == "waitany"))
    {
        return (Region::cCLASS_MPI
                | Region::cCAT_MPI_P2P
                | Region::cTYPE_MPI_WAIT
                | Region::cMODE_MPI_SINGLE);
    }
    if (  (name == "waitall")
       || (name == "waitsome"))
    {
        return (Region::cCLASS_MPI
                | Region::cCAT_MPI_P2P
                | Region::cTYPE_MPI_WAIT
                | Region::cMODE_MPI_MANY);
    }
    if (  (name.compare(0, 6, "buffer") == 0)
       || (name == "cancel")
       || (name == "get_count")
       || (name.find("probe") != string::npos)
       || (name.compare(0, 7, "request") == 0)
       || (name.compare(0, 5, "start") == 0))
    {
        return (Region::cCLASS_MPI
                | Region::cCAT_MPI_P2P);
    }

    // MPI I/O
    if (name.compare(0, 4, "file") == 0)
    {
        return (Region::cCLASS_MPI
                | Region::cCAT_MPI_IO);
    }

    // MPI RMA
    if (  (name == "accumulate")
       || (name == "put"))
    {
        return (Region::cCLASS_MPI
                | Region::cCAT_MPI_RMA
                | Region::cTYPE_MPI_RMA_COMM
                | Region::cMODE_RMA_PUT);
    }
    if (name == "get")
    {
        return (Region::cCLASS_MPI
                | Region::cCAT_MPI_RMA
                | Region::cTYPE_MPI_RMA_COMM
                | Region::cMODE_RMA_GET);
    }
    if (name.compare(0, 3, "win") == 0)
    {
        uint32_t result = Region::cCLASS_MPI | Region::cCAT_MPI_RMA;
        string   mode   = name.substr(4);

        if (mode == "fence")
        {
            return (result
                    | Region::cTYPE_MPI_RMA_COLL
                    | Region::cMODE_RMA_FENCE);
        }
        if (mode == "create")
        {
            return (result
                    | Region::cTYPE_MPI_RMA_COLL
                    | Region::cMODE_RMA_WIN_CREATE);
        }
        if (mode == "free")
        {
            return (result
                    | Region::cTYPE_MPI_RMA_COLL
                    | Region::cMODE_RMA_WIN_FREE);
        }
        if (mode == "start")
        {
            return (result
                    | Region::cTYPE_MPI_RMA_GATS
                    | Region::cMODE_RMA_START);
        }
        if (mode == "complete")
        {
            return (result
                    | Region::cTYPE_MPI_RMA_GATS
                    | Region::cMODE_RMA_COMPLETE);
        }
        if (mode == "post")
        {
            return (result
                    | Region::cTYPE_MPI_RMA_GATS
                    | Region::cMODE_RMA_POST);
        }
        if (mode == "wait")
        {
            return (result
                    | Region::cTYPE_MPI_RMA_GATS
                    | Region::cMODE_RMA_WAIT);
        }
        if (mode == "test")
        {
            return (result
                    | Region::cTYPE_MPI_RMA_GATS
                    | Region::cMODE_RMA_TEST);
        }
        if (mode == "lock")
        {
            return (result
                    | Region::cTYPE_MPI_RMA_PASSIVE
                    | Region::cMODE_RMA_LOCK);
        }
        if (mode == "unlock")
        {
            return (result
                    | Region::cTYPE_MPI_RMA_PASSIVE
                    | Region::cMODE_RMA_UNLOCK);
        }
    }

    return Region::cCLASS_MPI;
}
}    // unnamed namespace
