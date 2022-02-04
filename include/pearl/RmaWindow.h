/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef PEARL_RMAWINDOW_H
#define PEARL_RMAWINDOW_H


#include <stdint.h>

#include <iosfwd>


/*-------------------------------------------------------------------------*/
/**
 *  @file    RmaWindow.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class RmaWindow.
 *
 *  This header file provides the declaration of the class RmaWindow.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class Communicator;


/*-------------------------------------------------------------------------*/
/**
 *  @class   RmaWindow
 *  @ingroup PEARL_base
 *  @brief   Base class for remote memory access windows.
 *
 *  The class RmaWindow serves as a base class for all classes related to
 *  memory access windows. It is independent from the parallel-programming
 *  paradigm used, but always associated to a communicator.
 *
 *  The numerical identifiers of the individual RMA windows are globally
 *  defined and continuously enumerated, i.e., the ID is element of
 *  [0,@#windows-1].
 **/
/*-------------------------------------------------------------------------*/

class RmaWindow
{
    public:
        /// %RmaWindow definition identifier type.
        typedef uint32_t IdType;


        /// @name Constructors & destructor
        /// @{

        RmaWindow(IdType        id,
                  Communicator* communicator);

        virtual
        ~RmaWindow();

        /// @}
        /// @name Get window information
        /// @{

        IdType
        getId() const;

        Communicator*
        get_comm() const;

        /// @}


    private:
        /// Global RMA window identifier
        IdType m_id;

        /// Associated communicator
        Communicator* m_communicator;
};


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates RmaWindow
///
/// Prints the contents of the given RMA window definition object @a item to
/// an output @a stream in a human-readable form.
///
/// @param  stream  Output stream
/// @param  item    RMA window definition object
///
/// @return Output stream
///
std::ostream&
operator<<(std::ostream&    stream,
           const RmaWindow& item);

/// @}
}    // namespace pearl


#endif    // !PEARL_RMAWINDOW_H
