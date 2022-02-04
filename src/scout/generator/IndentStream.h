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


#ifndef SCOUT_GENERATOR_INDENTSTREAM_H
#define SCOUT_GENERATOR_INDENTSTREAM_H


#include <cstdio>
#include <sstream>


/*-------------------------------------------------------------------------*/
/**
 *  @brief Stream-like helper class to support proper indentation of code
 *         blocks.
 *
 *  The IndentStream class provides a C++ output stream-like interface to
 *  support writing properly indented code blocks to a C file pointer.
 *  During instantiation, the file pointer as well as the current indentation
 *  level has to be provided.  Any text or code block that is subsequently
 *  sent to the IndentStream is assumed to not have any leading whitespace on
 *  the first line, but otherwise be properly indented.  During destruction,
 *  each line of the whole text/code block is prefixed by an appropriate
 *  number of leading spaces according to the given indentation level, and
 *  then written to the file pointer.
 *
 *  @par Example
 *  @code
 *      #include <cstdio>
 *      #include <string>
 *
 *      #include "IndentStream.h"
 *
 *      using namespace std;
 *
 *      // ...
 *
 *      string body = "if (argc > 1)\n{\n    printf(\"Hello world!\\n\");\n}\n";
 *
 *      int indent = 0;
 *      IndentStream(stdout, indent)
 *          << "int main(int argc, char** argv)\n"
 *          << "{\n";
 *      indent++;
 *      IndentStream(stdout, indent)
 *          << body;
 *      indent--;
 *      IndentStream(stdout, indent)
 *          << "}\n";
 *
 *      // ...
 *  @endcode
 */
/*-------------------------------------------------------------------------*/

class IndentStream
{
    public:
        // --- Constants -----------------------------------

        /// Number of spaces to add per indentation level
        ///
        /// @todo[C++11] Use `constexpr` instead of `const`
        static const int INDENT_SPACES = 4;


        // --- Public member functions ---------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new IndentStream instance using the given file pointer
        /// @p filePtr and indentation level @p indent.
        ///
        /// @param filePtr
        ///     File pointer used for writing the result
        /// @param indent
        ///     Current indentation level
        ///
        IndentStream(std::FILE* filePtr,
                     int        indent)
            : mFilePtr(filePtr),
              mIndent(indent)
        {
        }

        /// @brief Destructor.
        ///
        /// Processes the accumulated code block to prefix each line with an
        /// appropriate number of spaces according to the indentation level
        /// specified during construction, writes the result to the file
        /// pointer, and destroys the instance.
        ///
        ~IndentStream();

        /// @}
        /// @name Output operators
        /// @{

        /// @brief Stream output operator
        ///
        /// Appends the textual representation of the given @p item to the
        /// internal construction buffer using the item's stream output
        /// operator.
        ///
        /// @param item
        ///     Data item to be sent
        /// @returns
        ///     Reference to the instance
        ///
        template< typename T >
        IndentStream&
        operator<<(const T& item)
        {
            mBuffer << item;

            return *this;
        }

        /// @}


    private:
        // --- Data members --------------------------------

        /// Text/code block construction buffer
        std::ostringstream mBuffer;

        /// File pointer
        std::FILE* mFilePtr;

        /// Current indentation level
        int mIndent;
};


#endif    // !SCOUT_GENERATOR_INDENTSTREAM_H
