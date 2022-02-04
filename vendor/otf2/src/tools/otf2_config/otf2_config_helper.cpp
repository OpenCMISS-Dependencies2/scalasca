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

#include <config.h>

#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <deque>
#include <string>

#include "otf2_config.hpp"

using namespace std;

/** Trim  and replace multiple white-spaces in @ str by a single one.
 *
 *  @param str              String to be processed.
 *
 *  @return Returns string where all multiple white-spaces are replaced
 *          by a single one.
 */
void
remove_multiple_whitespaces( string& str )
{
    string            search = "  "; // this string contains 2 spaces
    string::size_type pos;

    /* Trim */
    pos = str.find_last_not_of( ' ' );
    if ( pos != string::npos )
    {
        str.erase( pos + 1 );
        pos = str.find_first_not_of( ' ' );
        if ( pos != string::npos )
        {
            str.erase( 0, pos );
        }
    }
    else
    {
        str.erase( str.begin(), str.end() );
    }

    /* Remove multiple white-spaces */
    while ( ( pos = str.find( search ) ) != string::npos )
    {
        /* remove 1 character from the string at index */
        str.erase( pos, 1 );
    }

    return;
}

/** Replace all occurrences of @ pattern in string @ original by
 *  @ replacement.
 *
 *  @param pattern          String that should be replaced.
 *  @param replacement      Replacement for @ pattern.
 *  @param original         Input string.
 *
 *  @return Returns a string where all occurrences of @ pattern are
 *          replaced by @ replacement.
 */
void
replace_all( string&       str,
             const string& pattern,
             const string& replacement )
{
    string::size_type pos            = str.find( pattern, 0 );
    int               pattern_length = pattern.length();

    while ( pos != string::npos )
    {
        str.replace( pos, pattern_length, replacement );
        pos = str.find( pattern, 0 );
    }
}

/*
 * Returns a list of system path.
 */
deque<string>
convert_to_string_list( const string& input,
                        const string& delimiter )
{
    string        current_path = "";
    size_t        cur_pos      = 0;
    size_t        old_pos      = 0;
    deque<string> path_list;

    while ( cur_pos != string::npos )
    {
        cur_pos      = input.find( delimiter, old_pos );
        current_path = input.substr( old_pos, cur_pos - old_pos );
        path_list.push_back( current_path );
        old_pos = cur_pos + delimiter.length();
    }
    return path_list;
}

/**
 * Converts deque of strings into a string where all entries are space separated.
 */
string
deque_to_string( const deque<string>& input,
                 const string&        head,
                 const string&        delimiter,
                 const string&        tail )
{
    string                        output = head;
    deque<string>::const_iterator i;
    for ( i = input.begin(); i != input.end(); i++ )
    {
        output += delimiter + *i;
    }
    output += tail;
    return output;
}

string
strip_leading_whitespace( const string& input )
{
    const char* pos = input.c_str();
    while ( ( *pos != '\0' ) && ( ( *pos == ' ' ) || ( *pos == '\t' ) ) )
    {
        pos++;
    }
    return pos;
}

/**
 * Strips the head and leading delimiter from a input string.
 * Ignores leading whitespaces.
 */
string
strip_head( const string& input,
            const string& head_orig,
            const string& delimiter_orig )
{
    string output    = strip_leading_whitespace( input );
    string head      = strip_leading_whitespace( head_orig );
    string delimiter = strip_leading_whitespace( delimiter_orig );

    if ( output.compare( 0, head.length(), head ) == 0 )
    {
        output.erase( 0, head.length() );
    }

    output = strip_leading_whitespace( input );
    if ( output.compare( 0, delimiter.length(), delimiter ) == 0 )
    {
        output.erase( 0, delimiter.length() );
    }
    return output;
}

/**
 * Checks whether @a input contains @a item.
 */
bool
has_item( const deque<string>& input,
          const string&        item )
{
    deque<string>::const_iterator i;
    for ( i = input.begin(); i != input.end(); i++ )
    {
        if ( *i == item )
        {
            return true;
        }
    }
    return false;
}

/**
 * Removes dublicate entries from a deque container of strings. It keeps only the
 * last occurence of each entry. This ensures that the dependencies are maintained.
 */
deque<string>
remove_double_entries( const deque<string>& input )
{
    deque<string>                         output;
    deque<string>::const_reverse_iterator i;
    for ( i = input.rbegin(); i != input.rend(); i++ )
    {
        if ( !has_item( output, *i ) )
        {
            output.push_front( *i );
        }
    }

    return output;
}
