/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
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


#ifndef PEARL_MPIAMLISTENER_H
#define PEARL_MPIAMLISTENER_H


#include <pearl/AmListener.h>


/*-------------------------------------------------------------------------*/
/**
 * @file    MpiAmListener.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class MpiAmListener.
 *
 * This header files provides the declaration of the class
 * MpiAmListener.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{
class Communicator;
class MpiComm;


/*-------------------------------------------------------------------------*/
/**
 * @class   MpiAmListener
 * @ingroup PEARL_base
 * @brief   Listener using the communicator of an MpiWindow object
 *
 * Listeners can only be created using the static @a create() method
 * to ensure that the runtime system has full control over the object.
 *
 * @note It is implied that no other communication is taking place on the
 * communicator that could interfere with the active message communication.
 * This Listener will probe for and receive any message on this communicator.
 */
/*-------------------------------------------------------------------------*/

class MpiAmListener
    : public AmListener
{
    public:
        /// @name Constructors & destructor
        /// @{

        static AmListener::Ptr
        create(const Communicator& comm);

        virtual
        ~MpiAmListener();

        /// @}
        /// @name Handling incoming messages
        /// @{

        virtual bool
        probe();

        /// @}


    private:
        MpiAmListener(const MpiComm& comm);
};
}    // namespace pearl


#endif    // !PEARL_MPIAMLISTENER_H
