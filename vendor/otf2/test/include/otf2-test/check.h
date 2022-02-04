/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
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


/** @internal
 *
 *  @brief      Common test check functions.
 */


#ifndef OTF2_CHECK_H
#define OTF2_CHECK_H


static bool otf2_DEBUG;


static void
check_pointer( void* pointer,
               char* description,
               ... )
{
    va_list va;
    va_start( va, description );

    if ( pointer == NULL )
    {
        fprintf( stderr, "==ERROR== " );
        vfprintf( stderr, description, va );
        fprintf( stderr, "\n" );
        exit( EXIT_FAILURE );
    }

    if ( otf2_DEBUG )
    {
        printf( "==DEBUG== " );
        vfprintf( stdout, description, va );
        printf( "\n" );
    }

    va_end( va );
}


static void
check_status( OTF2_ErrorCode status,
              char*          description,
              ... )
{
    va_list va;
    va_start( va, description );

    if ( status != OTF2_SUCCESS )
    {
        fprintf( stderr, "==ERROR== %s: ", OTF2_Error_GetDescription( status ) );
        vfprintf( stderr, description, va );
        fprintf( stderr, "\n" );
        exit( EXIT_FAILURE );
    }

    if ( otf2_DEBUG )
    {
        printf( "==DEBUG== " );
        vfprintf( stdout, description, va );
        printf( "\n" );
    }

    va_end( va );
}


static void
check_condition( bool  condition,
                 char* description,
                 ... )
{
    va_list va;
    va_start( va, description );

    if ( !condition )
    {
        fprintf( stderr, "==ERROR== " );
        vfprintf( stderr, description, va );
        fprintf( stderr, "\n" );
        exit( EXIT_FAILURE );
    }

    if ( otf2_DEBUG )
    {
        printf( "==DEBUG== " );
        vfprintf( stdout, description, va );
        printf( "\n" );
    }

    va_end( va );
}


#endif /* OTF2_CHECK_H */
