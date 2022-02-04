/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SALSA_SALSAPARSER_H
#define SALSA_SALSAPARSER_H


#include <fstream>
#include <list>
#include <string>

#include "MessageChecker.h"
#include "salsa.h"


class SalsaParser
{
    public:
        SalsaParser(std::string inputfile,
                    bool        sparse_glob,
                    long        blocksize_glob,
                    formats     format_glob,
                    int         size)
            : sparse_glob(sparse_glob),
              blocksize_glob(blocksize_glob),
              format_glob(format_glob),
              size(size)
        {
            file.open(inputfile.c_str());
            line_number = 0;
        }

        int
        parseNext();

        funcs
        getFunc()
        {
            return stat_func;
        }

        std::string
        getFuncString()
        {
            return func;
        }

        modes
        getMode()
        {
            return stat_mode;
        }

        std::string
        getModeString()
        {
            return mode;
        }

        MessageChecker
        getMessageChecker()
        {
            return mc;
        }

        long
        getBlocksize()
        {
            return blocksize;
        }

        formats
        getFormat()
        {
            return format;
        }

        bool
        isSparse()
        {
            return sparse;
        }

        std::string
        getContraints()
        {
            return constraints_;
        }


    private:
        /* private fields */
        std::string func;
        std::string mode;

        funcs stat_func;
        modes stat_mode;

        bool        sparse_glob;
        bool        sparse;
        long        blocksize_glob;
        long        blocksize;
        formats     format_glob;
        formats     format;
        std::string constraints_;

        std::list< std::string >           tokens;
        std::list< std::string >::iterator it;

        MessageChecker mc;

        std::ifstream file;
        char          buffer[256];

        int size;
        int line_number;


        /* private methods */

        void
        string2tokens(const std::string&        str,
                      std::list< std::string >& token_list);

        void
        error(int err = 0);

        void
        lmode();

        void
        fmode();

        void
        funct();

        std::string
        type();

        void
        density();

        void
        output_format();

        void
        block();

        bool
        op(std::string s);

        void
        val(std::string s);

        void
        lval(std::string s);

        void
        constraint();

        void
        func_mode();
};


#endif    // !SALSA_SALSAPARSER_H
