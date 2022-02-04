/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "epk_conf.h"

#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>


int
epk_str2int(const char* val)
{
    if (!val)
    {
        return INT_MIN;
    }
    if (  (strcmp(val, "unlimited") == 0)
       || (strcmp(val, "Unlimited") == 0)
       || (strcmp(val, "UNLIMITED") == 0))
    {
        return INT_MAX;
    }

    char* ptr;
    long  result = strtol(val, &ptr, 10);
    if (  (ptr == val)
       || (*ptr != '\0')
       || (result < 0))
    {
        return INT_MIN;
    }
    if (result > INT_MAX)
    {
        return INT_MAX;
    }

    return result;
}


int
epk_str2bool(const char* str)
{
    char  strbuf[128];
    char* ptr = strbuf;

    strncpy(strbuf, str, 128);
    while (*ptr)
    {
        *ptr = tolower(*ptr);
        ++ptr;
    }

    if (  (strcmp(strbuf, "true") == 0)
       || (strcmp(strbuf, "yes") == 0)
       || (strcmp(strbuf, "on") == 0))
    {
        return 1;
    }

    long number = strtol(strbuf, &ptr, 10);
    if (  (ptr != strbuf)
       && (*ptr == '\0'))
    {
        return !!number;
    }

    return 0;
}
