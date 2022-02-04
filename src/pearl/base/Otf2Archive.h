/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2015                                                **
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
 *  @brief   Declaration of the class Otf2Archive.
 *
 *  This header file provides the declaration of the class Otf2Archive and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_OTF2ARCHIVE_H
#define PEARL_OTF2ARCHIVE_H


#include <otf2/OTF2_Reader.h>

#include <pearl/TraceArchive.h>


namespace pearl
{
namespace detail
{
/*-------------------------------------------------------------------------*/
/**
 *  @ingroup PEARL_base
 *  @brief   Representation of an OTF2 trace experiment archive.
 *
 *  The %Otf2Archive class provides a representation for trace experiment
 *  archives using the Open Trace Format 2 (OTF2). It implements the common
 *  interface defined by the %TraceArchive class, hiding the details of using
 *  the low-level OTF2 library. Instances of %Otf2Archive have to be created
 *  using the TraceArchive::open() factory method.
 **/
/*-------------------------------------------------------------------------*/

class Otf2Archive
    : public TraceArchive
{
    public:
        // --- Public member functions ---------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new OTF2 trace experiment archive handle and initializes
        /// its data members with the anchor file name @a anchorName and the
        /// experiment archive directory name @a archiveDirectory.
        ///
        /// @param anchorName
        ///     Name of the trace experiment archive anchor file
        /// @param archiveDir
        ///     Name of the trace experiment archive directory
        ///
        Otf2Archive(const std::string& anchorName,
                    const std::string& archiveDir);

        /// @brief Destructor.
        ///
        /// Implicitly closes the trace experiment archive, releases all
        /// allocated resources, and destroys the handle object.
        ///
        virtual
        ~Otf2Archive();

        /// @}


    private:
        // --- Data members --------------------------------

        /// OTF2 trace archive reader
        OTF2_Reader* mArchiveReader;

        /// Timer resolution in ticks per second
        uint64_t mTimerResolution;

        /// Global time offset (approximate timestamp of first event)
        uint64_t mGlobalOffset;


        // --- Private member functions & friends ----------

        virtual void
        openArchive();

        virtual void
        readDefinitions(GlobalDefs* defs);

        virtual void
        openContainer(const LocationGroup& locGroup);

        virtual void
        closeContainer();

        virtual LocalIdMaps*
        readIdMaps(const Location& location);

        virtual void
        readTrace(const GlobalDefs& defs,
                  const Location&   location,
                  LocalIdMaps*      idMaps,
                  LocalTrace*       trace);
};
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_OTF2ARCHIVE_H
