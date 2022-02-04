/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2020                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "Helper.h"

#include <algorithm>

#include "IndentStream.h"

using namespace std;


// --- Generic helper functions ---------------------------------------------

string
separatorComment(int           indent,
                 CommentStyle  style,
                 const string& text)
{
    // Prefix/suffix is based on comment style
    string prefix;
    string suffix;
    switch (style)
    {
        case C_STYLE:
            prefix = "/*---";
            suffix = "*/";
            break;

        case CPP_STYLE:
            prefix = "// ---";
            break;
    }

    // Calculate number of dashes needed after text
    int numDashes = 77
                    - indent * IndentStream::INDENT_SPACES
                    - prefix.length()
                    - text.length()
                    - (text.empty() ? 0 : 2)    // Spaces surrounding text
                    - suffix.length();

    // Compose comment
    string result(prefix);
    if (!text.empty())
    {
        result += ' ';
    }
    result += text;
    if (!text.empty())
    {
        result += ' ';
    }
    result += string(max(0, numDashes), '-');
    result += suffix;

    return result;
}
