/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2014                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SALSA_SALSA_H
#define SALSA_SALSA_H


#include <pearl/CallbackData.h>
#include <pearl/GlobalDefs.h>


enum funcs
{
    COUNT = 1,
    LENGTH,
    DURATION,
    RATE
};


enum modes
{
    MINIMUM = 1,
    MAXIMUM,
    AVERAGE,
    SUM
};


enum formats
{
    ASCII = 1,
    BINARY,
    SION
};


class CallbackDataneu
    : public pearl::CallbackData
{
    public:
        pearl::GlobalDefs* defs;
};


#endif    // !SALSA_SALSA_H
