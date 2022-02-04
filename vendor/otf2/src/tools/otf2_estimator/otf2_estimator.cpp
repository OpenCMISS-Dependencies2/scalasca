/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#include <config.h>

#include <string.h>
#include <iostream>
#include <sstream>
#include <string>

#include <otf2/OTF2_EventSizeEstimator.h>
#include <otf2/OTF2_GeneralDefinitions.h>
#define OTF2_TOOL_NAME "otf2-estimator"
#include <otf2_tools.hpp>

using namespace std;


static string
get_token( const string& input, size_t& pos )
{
    size_t end_pos = input.find_first_not_of( " \t", pos );
    if ( end_pos == string::npos )
    {
        pos = string::npos;
        return "";
    }

    pos = input.find_first_of( " \t", end_pos );
    return input.substr( end_pos, pos - end_pos );
}

#include "otf2_estimator_inc.cpp"

int
main( int argc, char** argv )
{
    if ( argc > 1 )
    {
        if ( strcmp( argv[ 1 ], "--help" ) == 0 || strcmp( argv[ 1 ], "-h" ) == 0 )
        {
            string usage =
                #include "otf2_estimator_usage.h"
            ;
            cout << usage.c_str() << endl;
            cout << "Report bugs to <" << PACKAGE_BUGREPORT << ">" << endl;
            exit( EXIT_SUCCESS );
        }
        else if ( strcmp( argv[ 1 ], "--version" ) == 0 || strcmp( argv[ 1 ], "-V" ) == 0 )
        {
            out( "version " << OTF2_VERSION );
            exit( EXIT_SUCCESS );
        }
        else
        {
            die( "unrecognized parameter: '" << argv[ 1 ] << "'" );
        }
    }

    OTF2_EventSizeEstimator* estimator = OTF2_EventSizeEstimator_New();

    while ( true )
    {
        if ( cin.eof() )
        {
            break;
        }

        string input;
        getline( cin, input );
        size_t pos           = 0;
        string current_token = get_token( input, pos );

        if ( current_token == "exit" )
        {
            break;
        }
        else if ( current_token == "get" )
        {
            handle_get( estimator, input, pos );
        }
        else if ( current_token == "set" )
        {
            handle_set( estimator, input, pos );
        }
        else if ( current_token == "list" )
        {
            current_token = get_token( input, pos );
            if ( current_token == "events" )
            {
                print_event_list();
            }
            else if ( current_token == "definitions" )
            {
                print_definition_list();
            }
            else if ( current_token == "types" )
            {
                print_type_list();
            }
            else
            {
                die( "can not list: '" << current_token << "'" );
            }
        }
        else if ( current_token != "" )
        {
            die( "unknown command: '" << current_token << "'" );
        }
    }

    OTF2_EventSizeEstimator_Delete( estimator );
    return EXIT_SUCCESS;
}
