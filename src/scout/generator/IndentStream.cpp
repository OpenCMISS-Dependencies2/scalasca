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

#include "IndentStream.h"

#include <string>

using namespace std;


// --- Constructors & destructor --------------------------------------------

IndentStream::~IndentStream()
{
    const string            input       = mBuffer.str();
    const string::size_type inputLength = input.length();

    string            result;
    string::size_type beginPos = 0;
    do
    {
        // Determine character index right after end of line
        string::size_type endPos = input.find('\n', beginPos);
        if (endPos == string::npos)
        {
            endPos = inputLength;
        }
        else
        {
            endPos++;
        }

        const string::size_type lineLength = endPos - beginPos;
        const string            line(input, beginPos, lineLength);

        // Indent line unless it is blank
        if (line.find_first_not_of(" \t\n\r\f\v") != string::npos)
        {
            result += string(mIndent * INDENT_SPACES, ' ');
        }
        result += line;

        beginPos += lineLength;
    }
    while (beginPos < inputLength);

    fprintf(mFilePtr, "%s", result.c_str());
}
