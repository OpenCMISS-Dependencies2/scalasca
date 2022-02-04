/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2014-2015                                                **
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
 *  @brief   Declaration of the scoped enumeration type Paradigm.
 *
 *  This header file provides the declaration of the scoped enumeration
 *  type Paradigm and related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_PARADIGM_H
#define PEARL_PARADIGM_H


#include <iosfwd>
#include <string>

#include <pearl/ScopedEnum.h>


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @struct  Paradigm_enum
 *  @ingroup PEARL_base
 *  @brief   Helper struct defining the enumerators of the Paradigm scoped
 *           enumeration.
 *
 *  The Paradigm_enum helper struct defines the enumerators provided by the
 *  scoped enumeration Paradigm.  It is passed as a base class to the
 *  ScopedEnum class template.
 **/
/*-------------------------------------------------------------------------*/

struct Paradigm_enum
{
    /// @brief Enumeration type defining all known paradigms.
    ///
    /// This enumeration type defines symbolic names for all known paradigms.
    /// A paradigm encodes the programming API a region definition, group
    /// definition, or event refers to (e.g., MPI or OpenMP), or how a region
    /// was captured (e.g., through compiler or user instrumentation).
    ///
    enum type
    {
        UNKNOWN,    ///< Unknown paradigm, used only if no other paradigm fits

        USER,               ///< Generated through user instrumentation
        COMPILER,           ///< Generated through compiler instrumentation
        OPENMP,             ///< OpenMP directives and API functions
        MPI,                ///< MPI API functions
        CUDA,               ///< CUDA API functions
        MEASUREMENT_SYSTEM, ///< Measurement-related activities
        PTHREAD,            ///< POSIX threads API functions
        HMPP,               ///< HMPP runtime functions
        OMPSS,              ///< OmpSs directives
        HARDWARE,           ///< Generated through hardware events
        GASPI,              ///< GASPI API functions
        UPC,                ///< Generated through UPC instrumentation
        SHMEM,              ///< SHMEM API functions
        WINTHREAD,          ///< Windows threads API functions
        QTTHREAD,           ///< Qt threads API functions
        ACETHREAD,          ///< ACE threads API functions
        TBBTHREAD,          ///< Intel Threading Building Blocks (TBB) API functions
        OPENACC,            ///< OpenACC directives and API functions
        OPENCL,             ///< OpenCL API functions and kernels
        MTAPI,              ///< Multicore Association threading API (MTAPI) functions
        SAMPLING,           ///< Functions recorded by sampling
        NONE,               ///< An entity that does not belong to a specific paradigm

        NUMBER_OF_PARADIGMS    ///< For internal use only, not a valid paradigm
    };
};


// --- Type definitions -----------------------------------------------------

/// @copydoc Paradigm_enum::type
/// See Paradigm_enum::type for details.
typedef ScopedEnum< Paradigm_enum > Paradigm;


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief Stream output operator.
///
/// Prints the contents of the given paradigm enumeration object @a item to
/// an output @a stream in a human-readable form.
///
/// @param stream
///     Output stream
/// @param item
///     Paradigm enumeration object
/// @returns
///     Output stream
///
std::ostream&
operator<<(std::ostream&   stream,
           const Paradigm& item);

/// @}
/// @name Conversion functions
/// @{

/// @brief Get string representation of a paradigm object.
///
/// Returns a human-readable string representation of the given paradigm
/// enumeration object @a item.
///
/// @param item
///     Paradigm enumeration object
/// @returns
///     Corresponding string representation
///
std::string
toString(const Paradigm& item);

/// @}
}    // namespace pearl


#endif    // !PEARL_PARADIGM_H
