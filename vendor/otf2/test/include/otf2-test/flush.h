/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/** @internal
 *
 *  @brief      Default flush callbacks.
 */


#ifndef OTF2_TEST_FLUSH_H
#define OTF2_TEST_FLUSH_H

static OTF2_FlushType
otf2_test_pre_flush( void*         userData,
                     OTF2_FileType fileType,
                     uint64_t      locationId,
                     void*         callerData,
                     bool          final )
{
    return OTF2_TEST_FLUSH_DEFAULT;
}

#ifdef OTF2_TEST_FLUSH_GETTIME

static OTF2_TimeStamp
otf2_test_post_flush( void*         userData,
                      OTF2_FileType fileType,
                      uint64_t      locationId )
{
    return OTF2_TEST_FLUSH_GETTIME();
}

#endif

static OTF2_FlushCallbacks otf2_test_flush_callbacks =
{
    .otf2_pre_flush  = otf2_test_pre_flush,
#ifdef OTF2_TEST_FLUSH_GETTIME
    .otf2_post_flush = otf2_test_post_flush
#else
    .otf2_post_flush = NULL
#endif
};

#endif /* OTF2_TEST_FLUSH_H */
