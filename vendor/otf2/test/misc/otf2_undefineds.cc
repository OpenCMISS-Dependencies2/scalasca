/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */



/**
 * @file
 */


#include <config.h>


#include <stdlib.h>
#include <stdio.h>

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#include <stdint.h>


#include <CuTest.h>


#include <otf2/OTF2_GeneralDefinitions.h>


#if defined( UINT8_MAX ) && defined( INT8_MIN )
void
test_1( CuTest* tc )
{
    CuAssert( tc, "UINT8", OTF2_UNDEFINED_UINT8 == UINT8_MAX );
    CuAssert( tc, "INT8", OTF2_UNDEFINED_INT8 == INT8_MIN );
}
#endif


#if defined( UINT16_MAX ) && defined( INT16_MIN )
void
test_2( CuTest* tc )
{
    CuAssert( tc, "UINT16", OTF2_UNDEFINED_UINT16 == UINT16_MAX );
    CuAssert( tc, "INT16", OTF2_UNDEFINED_INT16 == INT16_MIN );
}
#endif


#if defined( UINT32_MAX ) && defined( INT32_MIN )
void
test_3( CuTest* tc )
{
    CuAssert( tc, "UINT32", OTF2_UNDEFINED_UINT32 == UINT32_MAX );
    CuAssert( tc, "INT32", OTF2_UNDEFINED_INT32 == INT32_MIN );
}
#endif


#if defined( UINT64_MAX ) && defined( INT64_MIN )
void
test_4( CuTest* tc )
{
    CuAssert( tc, "UINT64", OTF2_UNDEFINED_UINT64 == UINT64_MAX );
    CuAssert( tc, "INT64", OTF2_UNDEFINED_INT64 == INT64_MIN );
}
#endif


int
main()
{
    CuUseColors();
    CuString* output = CuStringNew();
    CuSuite*  suite  = CuSuiteNew( "OTF2_UNDEFINED C++" );

#if defined( UINT8_MAX ) && defined( INT8_MIN )
    SUITE_ADD_TEST_NAME( suite, test_1, "(u)int8_t" );
#endif
#if defined( UINT16_MAX ) && defined( INT16_MIN )
    SUITE_ADD_TEST_NAME( suite, test_2, "(u)int16_t" );
#endif
#if defined( UINT32_MAX ) && defined( INT32_MIN )
    SUITE_ADD_TEST_NAME( suite, test_3, "(u)int32_t" );
#endif
#if defined( UINT64_MAX ) && defined( INT64_MIN )
    SUITE_ADD_TEST_NAME( suite, test_4, "(u)int64_t" );
#endif

    CuSuiteRun( suite );
    CuSuiteSummary( suite, output );

    int failCount = suite->failCount;
    if ( failCount )
    {
        printf( "%s", output->buffer );
    }

    CuSuiteFree( suite );
    CuStringFree( output );

    return failCount ? EXIT_FAILURE : EXIT_SUCCESS;
}
