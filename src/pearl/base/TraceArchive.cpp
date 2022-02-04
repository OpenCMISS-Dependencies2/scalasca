/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
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
 *  @brief   Implementation of the class TraceArchive.
 *
 *  This file provides the implementation of the class TraceArchive and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/TraceArchive.h>

#include <sys/stat.h>

#include <cassert>
#include <cstddef>

#include <pearl/Error.h>
#include <pearl/LocalTrace.h>
#include <pearl/ScopedPtr.h>

#include "EpikArchive.h"
#include "LocalIdMaps.h"
#include "MutableGlobalDefs.h"
#include "Otf2Archive.h"
#include "epk_archive.h"

using namespace std;
using namespace pearl;
using namespace pearl::detail;


// --- Constructors & destructor --------------------------------------------

TraceArchive::TraceArchive(const string& anchorName,
                           const string& archiveDir)
    : mAnchorName(anchorName),
      mArchiveDirectory(archiveDir)
{
}


TraceArchive::~TraceArchive()
{
}


// --- Archive management ---------------------------------------------------

TraceArchive*
TraceArchive::open(const string& anchorName)
{
    // --- STEP 1: Canonicalize anchor name ---

    string canonicalName(anchorName);

    // Remove trailing slashes
    string::size_type length = canonicalName.length();
    while (  !canonicalName.empty()
          && (canonicalName[length - 1] == '/'))
    {
        --length;
        canonicalName.erase(length);
    }

    // Sanity check
    if (canonicalName.empty())
    {
        throw RuntimeError("Empty experiment archive name provided!");
    }

    // Ensure that we have at least one directory separator
    if (canonicalName[0] != '/')
    {
        canonicalName.insert(0, "./");
    }

    // --- STEP 2: Check whether anchor is a directory ---

    struct stat statBuffer;
    if (stat(canonicalName.c_str(), &statBuffer))
    {
        throw RuntimeError("Error accessing experiment archive \""
                           + anchorName + "\"!");
    }
    bool isDirectory = S_ISDIR(statBuffer.st_mode);

    // --- STEP 3: Determine basename/dirname ---

    string::size_type index = canonicalName.find_last_of('/');
    string            baseName(canonicalName, index + 1);
    string            dirName(canonicalName);
    if (!isDirectory)
    {
        dirName.erase(index);
    }

    // Sanity check
    if (baseName.length() <= 5)
    {
        throw RuntimeError("Invalid experiment archive name \""
                           + anchorName + "\"!");
    }

    // --- STEP 4: Create delegate ---

    ScopedPtr< TraceArchive > archive;
    if (baseName.compare(baseName.length() - 5, 5, ".otf2") == 0)
    {
        archive.reset(new Otf2Archive(canonicalName, dirName));
    }
    else if (baseName.compare(0, 5, "epik_") == 0)
    {
        archive.reset(new EpikArchive(dirName));
    }
    else
    {
        throw RuntimeError("Unknown experiment archive format \""
                           + anchorName + "\"!");
    }

    // --- STEP 5: Open the experiment archive ---

    assert(archive);
    archive->openArchive();

    return archive.release();
}


// --- Access archive properties --------------------------------------------

string
TraceArchive::getAnchorName() const
{
    return mAnchorName;
}


string
TraceArchive::getArchiveDirectory() const
{
    return mArchiveDirectory;
}


// --- Access global definition data ----------------------------------------

GlobalDefs*
TraceArchive::getDefinitions()
{
    ScopedPtr< MutableGlobalDefs > definitions(new MutableGlobalDefs);
    readDefinitions(definitions.get());
    definitions->validate();
    definitions->setup();

    return definitions.release();
}


// --- Access local mapping & event trace data ------------------------------

void
TraceArchive::openTraceContainer(const LocationGroup& locGroup)
{
    openContainer(locGroup);
}


void
TraceArchive::closeTraceContainer()
{
    closeContainer();
}


LocalIdMaps*
TraceArchive::getIdMaps(const Location& location)
{
    return readIdMaps(location);
}


LocalTrace*
TraceArchive::getTrace(const GlobalDefs& defs,
                       const Location&   location,
                       LocalIdMaps*      idMaps)
{
    ScopedPtr< LocalIdMaps > mapData(idMaps);
    if (!mapData)
    {
        mapData.reset(getIdMaps(location));
    }

    ScopedPtr< LocalTrace > trace(new LocalTrace(defs, location));
    readTrace(defs, location, mapData.get(), trace.get());

    return trace.release();
}
