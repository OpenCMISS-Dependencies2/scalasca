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
 * Copyright (c) 2009-2012,
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


// order matters!
// PGI 18.4+ on Power misbehaves here, the explicit cast to the type is a
// secondary counter measure
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


#include <CuTest.h>


#include <otf2/OTF2_GeneralDefinitions.h>


void
test_1( CuTest* tc )
{
    CuAssert( tc, "UINT8", OTF2_UNDEFINED_UINT8 == ( uint8_t )UINT8_MAX );
    CuAssert( tc, "INT8", OTF2_UNDEFINED_INT8 == ( int8_t )INT8_MIN );
}


void
test_2( CuTest* tc )
{
    CuAssert( tc, "UINT16", OTF2_UNDEFINED_UINT16 == ( uint16_t )UINT16_MAX );
    CuAssert( tc, "INT16", OTF2_UNDEFINED_INT16 == ( int16_t )INT16_MIN );
}


void
test_3( CuTest* tc )
{
    CuAssert( tc, "UINT32", OTF2_UNDEFINED_UINT32 == ( uint32_t )UINT32_MAX );
    CuAssert( tc, "INT32", OTF2_UNDEFINED_INT32 == ( int32_t )INT32_MIN );
}


void
test_4( CuTest* tc )
{
    CuAssert( tc, "UINT64", OTF2_UNDEFINED_UINT64 == ( uint64_t )UINT64_MAX );
    CuAssert( tc, "INT64", OTF2_UNDEFINED_INT64 == ( int64_t )INT64_MIN );
}


int
main()
{
    CuUseColors();
    CuString* output = CuStringNew();
    CuSuite*  suite  = CuSuiteNew( "OTF2_UNDEFINED C" );

    SUITE_ADD_TEST_NAME( suite, test_1, "(u)int8_t" );
    SUITE_ADD_TEST_NAME( suite, test_2, "(u)int16_t" );
    SUITE_ADD_TEST_NAME( suite, test_3, "(u)int32_t" );
    SUITE_ADD_TEST_NAME( suite, test_4, "(u)int64_t" );

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
