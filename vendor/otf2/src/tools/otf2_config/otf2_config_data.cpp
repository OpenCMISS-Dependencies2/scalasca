/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2015, 2017,
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

/**
 * @file
 *
 * Implements the representation and analysis of library dependencies
 */

#include <config.h>

#include <iostream>
#include <stdlib.h>
#include <cstring>

#include "otf2_config.hpp"

#include "otf2_config_data.hpp"

using namespace std;

/* ****************************************************************************
                                                         class otf2_config_data
******************************************************************************/

otf2_config_data::otf2_config_data( void )
{
}

otf2_config_data::~otf2_config_data()
{
}

string
otf2_config_data::GetLibraries( const deque<string>& inputLibs )
{
    /* Traversing backwards will add the -l flags from the otf2_* lib last.
       this makes the system more robust against broken dependencies in installed
       .la files of other libraries, in particular libbfd.
       During configure we tested whether linking works and this is more reliable
       than installed .la files. */
    deque<string>                   deps = get_dependencies( inputLibs );
    deque<string>                   libs;
    deque<string>::reverse_iterator i;

    for ( i = deps.rbegin(); i != deps.rend(); i++ )
    {
        la_object obj = m_la_objects[ *i ];
        libs.push_front( "-l" + obj.m_lib_name.substr( 3 ) );
        libs.insert( libs.end(),
                     obj.m_libs.begin(),
                     obj.m_libs.end() );
    }
    libs = remove_double_entries( libs );

    return deque_to_string( libs, "", " ", "" );
}

string
otf2_config_data::GetLDFlags( const deque<string>& libs )
{
    deque<string>           deps = get_dependencies( libs );
    deque<string>           flags;
    deque<string>::iterator i;

    for ( i = deps.begin(); i != deps.end(); i++ )
    {
        la_object obj = m_la_objects[ *i ];
        flags.push_back( "-L" + obj.m_install_dir );
        flags.insert( flags.end(),
                      obj.m_ldflags.begin(),
                      obj.m_ldflags.end() );
    }
    flags = remove_double_entries( flags );

    return deque_to_string( flags, "", " ", "" );
}

string
otf2_config_data::GetRpathFlags( const deque<string>& libs )
{
    deque<string>           deps = get_dependencies( libs );
    deque<string>           flags;
    deque<string>::iterator i;
    deque<string>::iterator j;

    for ( i = deps.begin(); i != deps.end(); i++ )
    {
        la_object obj = m_la_objects[ *i ];
        flags.push_back( obj.m_install_dir );
        for ( j = obj.m_rpath.begin(); j != obj.m_rpath.end(); j++ )
        {
            size_t index = j->find( "-R" );
            if ( index == 0 )
            {
                j->replace( index, strlen( "-R" ), "" );
            }
            flags.push_back( strip_head( *j, m_rpath_head, m_rpath_delimiter ) );
        }
    }
    flags = remove_double_entries( flags );
    flags = remove_system_path( flags );

    return deque_to_string( flags,
                            m_rpath_head,
                            m_rpath_delimiter,
                            m_rpath_tail );
}

void
otf2_config_data::prepare_rpath_flags( void )
{
    // Replace $wl by LIBDIR_FLAG_WL and erase everything from
    // $libdir on in order to create m_rpath_head and
    // m_rpath_delimiter. This will work for most and for the relevant
    // (as we know in 2012-07) values of LIBDIR_FLAG_CC. Some possible
    // values are (see also ticket 530,
    // https://silc.zih.tu-dresden.de/trac-silc/ticket/530):
    // '+b $libdir'
    // '-L$libdir'
    // '-R$libdir'
    // '-rpath $libdir'
    // '$wl-blibpath:$libdir:'"$aix_libpath"
    // '$wl+b $wl$libdir'
    // '$wl-R,$libdir'
    // '$wl-R $libdir:/usr/lib:/lib'
    // '$wl-rpath,$libdir'
    // '$wl--rpath $wl$libdir'
    // '$wl-rpath $wl$libdir'
    // '$wl-R $wl$libdir'
    // For a complete list, check the currently used libtool.m4.

    string        rpath_flag = m_rpath_flag_cc;
    const string& wl         = m_wl_flag;
    const string& aix_path   = m_aix_libpath;
    size_t        index      = 0;
    while ( true )
    {
        index = rpath_flag.find( "$wl", index );
        if ( index == string::npos )
        {
            break;
        }
        rpath_flag.replace( index, strlen( "$wl" ), wl );
        ++index;
    }
    index = rpath_flag.find( "$libdir", 0 );
    if ( index != string::npos )
    {
        rpath_flag.erase( index );
    }

    bool is_aix = ( !aix_path.empty() );
    if ( is_aix )
    {
        m_rpath_head      = " " + rpath_flag;
        m_rpath_delimiter = ":";
        m_rpath_tail      = ":" + aix_path;
    }
    else
    {
        m_rpath_head      = "";
        m_rpath_delimiter = " " + rpath_flag;
        m_rpath_tail      = "";
    }
}

deque<string>
otf2_config_data::remove_system_path( const deque<string>& pathList )
{
    const string& dlsearch_path = m_sys_lib_dlsearch_path;
    deque<string> system_paths  = convert_to_string_list( dlsearch_path, " " );
    deque<string> result_paths;

    deque<string>::iterator       sys_path;
    deque<string>::const_iterator app_path;

    for ( app_path = pathList.begin();
          app_path != pathList.end();
          ++app_path )
    {
        bool is_sys_path = false;
        for ( sys_path = system_paths.begin();
              sys_path != system_paths.end();
              ++sys_path )
        {
            if ( *app_path == *sys_path )
            {
                is_sys_path = true;
            }
        }
        if ( !is_sys_path )
        {
            result_paths.push_back( *app_path );
        }
    }
    return result_paths;
}

string
otf2_config_data::append_ld_run_path_to_rpath( const string& rpath )
{
    /* Only necessary to decide whether we are on AIX or not */
    const string& aix_path = m_aix_libpath;

    /* Get variable values */
    const char* ld_run_path = getenv( "LD_RUN_PATH" );
    if ( ld_run_path == NULL || *ld_run_path == '\0' )
    {
        return rpath;
    }

    deque<string> path_list = convert_to_string_list( ld_run_path, ":" );
    path_list = remove_system_path( path_list );

    /* On AIX ist just a colon separated list, after a head */
    if ( !aix_path.empty() )
    {
        if ( rpath == "" )
        {
            return deque_to_string( path_list,
                                    m_rpath_head,
                                    m_rpath_delimiter,
                                    "" );
        }
        return deque_to_string( path_list,
                                rpath,
                                m_rpath_delimiter,
                                "" );
    }

    /* Otherwise replace all colons by the rpath flags */
    return deque_to_string( path_list,
                            rpath,
                            m_rpath_delimiter,
                            "" );
}

deque<string>
otf2_config_data::get_dependencies( const deque<string>& libs )
{
    deque<string> deps = libs;

    for ( size_t i = 0; i < deps.size(); i++ )
    {
        if ( m_la_objects.find( deps[ i ] ) == m_la_objects.end() )
        {
            die( "cannot resolve dependency \"" << deps[ i ] << "\"" );
        }
        la_object obj = m_la_objects[ deps[ i ] ];

        deps.insert( deps.end(),
                     obj.m_dependency_las.begin(),
                     obj.m_dependency_las.end() );
    }
    return remove_double_entries( deps );
}

string
otf2_config_data::prepare_string( const string& str )
{
    string       result( str );
    const string pattern1 = " ";
    const string replace1 = ",";
    const string pattern2 = m_wl_flag;
    const string replace2 = "";

    remove_multiple_whitespaces( result );
    /* Replace all white-spaces by comma */
    replace_all( result, pattern1, replace1 );
    /* Replace flag for passing arguments to linker through compiler
     * (flags not needed beacause we use '-Xlinker' to specify linker
     * flags when using CUDA compiler */
    if ( pattern2.length() != 0 )
    {
        replace_all( result, pattern2, replace2 );
    }

    return result;
}
