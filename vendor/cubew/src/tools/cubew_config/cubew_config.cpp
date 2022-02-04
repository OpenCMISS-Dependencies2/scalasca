/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2009-2018                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2009-2011                                                **
**  RWTH Aachen University, JARA-HPC                                       **
**                                                                         **
**  Copyright (c) 2009-2016                                                **
**  TU Dresden, Zentrum fuer Informationsdienste und Hochleistungsrechnen  **
**                                                                         **
**  Copyright (c) 2009-2013                                                **
**  Technische Universitaet Muenchen, Germany                              **
**                                                                         **
**  Copyright (c) 2009-2013                                                **
**  University of Oregon, Eugene, USA                                      **
**                                                                         **
**  Copyright (c) 2009-2013                                                **
**  Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany      **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include "config.h"
#include "cubew-version.h"

#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <deque>
#include <string>

#include "cubew_config.hpp"

#include "cubew_config_data.hpp"

using namespace std;

enum
{
    NO_ACTION           = 0,
    ACTION_LIBS         = 1,
    ACTION_CFLAGS       = 2,
    ACTION_LDFLAGS      = 3,
    ACTION_CPPFLAGS     = 4,
    ACTION_INCLDIR      = 5,
    ACTION_CC           = 6,
    ACTION_SUBSTRATES   = 7,
    ACTION_COMPRESSIONS = 8,
    ACTION_TARGETS      = 9,
    ACTION_SUMMARY      = 10
};

enum
{
    TARGET_FRONTEND = 0,
    TARGET_BACKEND  = 1,
    TARGET_MIC      = 2
};

/* ___ Prototypes for static functions. _____________________________________ */

/* *INDENT-OFF* */
#if HAVE( MIC_SUPPORT )
static void delegate( int argc, char** argv, const string& target );
#endif

static string target_to_string( int target );
/* *INDENT-ON* */


int
main( int    argc,
      char** argv )
{
    int         action = NO_ACTION;
    int         target = TARGET_FRONTEND;
    bool        nvcc   = false;
    std::string summary_command( "cat " PKGDATADIR "/" PACKAGE "/cubew.summary" );

    /* parsing the command line */
    for ( int i = 1; i < argc; i++ )
    {
        if ( strcmp( argv[ i ], "--help" ) == 0 || strcmp( argv[ i ], "-h" ) == 0 )
        {
            string usage =
                #include "cubew_config_usage.h"
            ;
            cout << usage.c_str() << endl;
            cout << "Report bugs to <" << PACKAGE_BUGREPORT << ">" << endl;
            exit( EXIT_SUCCESS );
        }
        else if ( strcmp( argv[ i ], "--version" ) == 0 )
        {
            out( "version " << CUBEW_VERSION );
            exit( EXIT_SUCCESS );
        }
        else if ( strcmp( argv[ i ], "--cubew-revision" ) == 0 )
        {
            cout << CUBEW_REVISION << endl;
            exit( EXIT_SUCCESS );
        }
//        else if ( strcmp( argv[ i ], "--common-revision" ) == 0 )
//        {
//            cout << SCOREP_COMMON_REVISION << endl;
//            exit( EXIT_SUCCESS );
//        }
        else if ( strcmp( argv[ i ], "--interface-version" ) == 0 )
        {
            cout << LIBRARY_INTERFACE_VERSION << endl;
            exit( EXIT_SUCCESS );
        }
        else if ( strcmp( argv[ i ], "--libs" ) == 0 )
        {
            action = ACTION_LIBS;
        }
        else if ( strcmp( argv[ i ], "--cflags" ) == 0 )
        {
            action = ACTION_CFLAGS;
        }
        else if ( strcmp( argv[ i ], "--ldflags" ) == 0 )
        {
            action = ACTION_LDFLAGS;
        }
        else if ( strcmp( argv[ i ], "--cppflags" ) == 0 )
        {
            action = ACTION_CPPFLAGS;
        }
        else if ( strcmp( argv[ i ], "--include" ) == 0 )
        {
            action = ACTION_INCLDIR;
        }
        else if ( strcmp( argv[ i ], "--cc" ) == 0 )
        {
            action = ACTION_CC;
        }
        else if ( !strcmp( argv[ i ], "--features" )
                  || !strncmp( argv[ i ], "--features=", 11 ) )
        {
            const char* category = 0;
            if ( argv[ i ][ 10 ] == '=' )
            {
                category = argv[ i ] + 11;
            }
            else
            {
                category = argv[ i + 1 ];
                if ( !category )
                {
                    die( "missing argument for --feature" );
                }
                i++;
            }

            if ( !strcmp( category, "help" ) )
            {
                out( "available feature categories:" );
                cout << "substrates" << endl;
                cout << "compressions" << endl;
                cout << "targets" << endl;
                exit( EXIT_SUCCESS );
            }
            if ( !strcmp( category, "substrates" ) )
            {
                action = ACTION_SUBSTRATES;
            }
            else if ( !strcmp( category, "compressions" ) )
            {
                action = ACTION_COMPRESSIONS;
            }
            else if ( !strcmp( category, "targets" ) )
            {
                action = ACTION_TARGETS;
            }
            else
            {
                die( "unknown feature category: " << category );
            }
        }
        else if ( !strcmp( argv[ i ], "--target" )
                  || !strncmp( argv[ i ], "--target=", 9 ) )
        {
            const char* target_name = 0;
            if ( argv[ i ][ 8 ] == '=' )
            {
                target_name = argv[ i ] + 9;
            }
            else
            {
                target_name = argv[ i + 1 ];
                if ( !target_name )
                {
                    die( "missing argument for --target" );
                }
                i++;
            }

            if ( !strcmp( target_name, "frontend" ) )
            {
                target = TARGET_FRONTEND;
            }
            else if ( !strcmp( target_name, "backend" ) )
            {
#if !HAVE( PLATFORM_MIC )
                target = TARGET_BACKEND;
#endif          /* !HAVE( PLATFORM_MIC ) */
            }
            else if ( !strcmp( target_name, "mic" ) )
            {
                target = TARGET_MIC;
            }
            else
            {
                die( "unknown target: " << target_name );
            }
        }
        else if ( strcmp( argv[ i ], "--backend" ) == 0 )
        {
            warn( "the '--backend' option is deprecated" );
#if !HAVE( PLATFORM_MIC )
            target = TARGET_BACKEND;
#endif      /* !HAVE( PLATFORM_MIC ) */
        }
        else if ( strcmp( argv[ i ], "--cuda" ) == 0 )
        {
            warn( "the '--cuda' option is deprecated" );
            nvcc = true;
        }
        else if ( strcmp( argv[ i ], "--config-summary" ) == 0 )
        {
            action = ACTION_SUMMARY;
        }

        else
        {
            die( "unrecognized option: '" << argv[ i ] << "'" );
        }
    }

    if ( NO_ACTION == action )
    {
        die( "no command specified" );
    }

    /* Handle --features==targets first before delegating
       to targets cubew_config (if applicable) */
    if ( ACTION_TARGETS == action )
    {
#if defined CROSS_BUILD && !HAVE( PLATFORM_MIC )
        cout << "frontend" << endl;
#endif  /* HAVE ( CROSS_BUILD ) && !HAVE( PLATFORM_MIC ) */
        cout << "backend" << endl;
#if HAVE( PLATFORM_MIC ) || HAVE( MIC_SUPPORT )
        cout << "mic" << endl;
#endif  /* HAVE( PLATFORM_MIC ) || HAVE( MIC_SUPPORT ) */
        return 0;
    }

    cubew_config_data* data = 0;
    switch ( target )
    {
        case TARGET_FRONTEND:
            data = cubew_config_get_frontend_data();
            break;

        case TARGET_BACKEND:
            data = cubew_config_get_backend_data();
            break;

#if HAVE( PLATFORM_MIC )
        case TARGET_MIC:
            data = cubew_config_get_backend_data();
            break;
#endif  /* HAVE( PLATFORM_MIC */

#if HAVE( MIC_SUPPORT )
        case TARGET_MIC:
            delegate( argc, argv, "mic" );
            break;
#endif  /* HAVE( MIC_SUPPORT ) */

        default:
            die( "unsupported target: '" << target_to_string( target ) << "'" );
            break;
    }

    string        str;
    string        temp;
    deque<string> libs;
    libs.push_back( "libcube4w" );

    switch ( action )
    {
        case ACTION_LIBS:
            cout << data->GetLibraries( libs ) << endl;
            break;

        case ACTION_LDFLAGS:
            cout << data->GetLDFlags( libs );
            if ( data->m_use_rpath_flag )
            {
                str = data->GetRpathFlags( libs );
                str = data->append_ld_run_path_to_rpath( str );
            }
            if ( nvcc )
            {
                str = " -Xlinker " + data->prepare_string( str );
            }

            cout << str << endl;
            break;

        case ACTION_CFLAGS:
        case ACTION_CPPFLAGS:
            str += data->m_cppflags;
            if ( nvcc )
            {
                /* Construct string in multiple steps to please IBM compiler. */
                temp = data->prepare_string( str );
                str  = " -Xcompiler ";
                str += temp.c_str();
            }

            cout << str.c_str() << endl;
            break;

        case ACTION_INCLDIR:
            str += data->m_incldir;
            cout << str.c_str() << endl;
            break;

        case ACTION_CC:
            cout << data->m_cc << endl;
            break;

        case ACTION_SUMMARY:
            if ( system( summary_command.c_str() ) != 0 )
            {
                std::cerr << "Error executing: " << summary_command << std::endl;
            }
            break;

        case ACTION_SUBSTRATES:
            cout << "none" << endl;
            break;

        case ACTION_COMPRESSIONS:
#ifdef COMPRESSION
            cout << "zlib" << endl;
#else
            cout << "none" << endl;
#endif
            break;

        case ACTION_TARGETS:
            cout << "backend" << endl;
#if HAVE( PLATFORM_MIC ) || HAVE( MIC_SUPPORT )
            cout << "mic" << endl;
#endif  /* HAVE( PLATFORM_MIC ) || HAVE( MIC_SUPPORT ) */

            break;
    }

    return 0;
}

#if HAVE( MIC_SUPPORT )
void
delegate( int argc, char** argv, const string& target )
{
    // Construct command string with all original arguments
    string command = PKGLIBEXECDIR "/cubew-config-" + target;
    for ( int i = 1; i < argc; ++i )
    {
        command += " " + string( argv[ i ] );
    }

    // Delegate to cubew-config command of target build
    exit( system( command.c_str() ) );
}
#endif

string
target_to_string( int target )
{
    switch ( target )
    {
        case TARGET_MIC:
            return "mic";
    }
    return "unknown";
}
