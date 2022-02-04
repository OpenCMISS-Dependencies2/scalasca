/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2014-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Implementation of functions related to the scoped enumeration
 *           type Paradigm.
 *
 *  This file provides the implementation of functions related to the scoped
 *  enumeration type Paradigm.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include <pearl/Paradigm.h>

#include <cassert>
#include <iostream>

using namespace std;


// --- Related functions ----------------------------------------------------

namespace pearl
{
// --- Stream I/O functions --------------------------------

ostream&
operator<<(ostream&        stream,
           const Paradigm& item)
{
    return stream << toString(item);
}


// --- Conversion functions --------------------------------

string
toString(const Paradigm& item)
{
    // Ensure that the item's value is in range
    assert(  (item >= Paradigm::UNKNOWN)
          && (item < Paradigm::NUMBER_OF_PARADIGMS));

    // Look up string representation
    static const char* const string_reps[] = {
        "unknown",
        "user",
        "compiler",
        "openmp",
        "mpi",
        "cuda",
        "measurement",
        "pthread",
        "hmpp",
        "ompss",
        "hardware",
        "gaspi",
        "upc",
        "shmem",
        "winthread",
        "qtthread",
        "acethread",
        "tbbthread",
        "openacc",
        "opencl",
        "mtapi",
        "sampling",
        "none"
    };

    return string_reps[native_value(item)];
}
}    // namespace pearl
