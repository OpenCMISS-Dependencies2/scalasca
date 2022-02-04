/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
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


#ifndef PEARL_AMLISTENER_H
#define PEARL_AMLISTENER_H


#include <cstring>

#include <pearl/CountedPtr.h>
#include <pearl/Uncopyable.h>


/*-------------------------------------------------------------------------*/
/**
 * @file    AmListener.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class AmListener.
 *
 * This header files provides the declaration of the class
 * AmListener. This is the abstract base class used by the
 * runtime.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{
// Forward declarations
class Buffer;
class Communicator;
class AmListener;
class AmRuntime;


/*-------------------------------------------------------------------------*/
/**
 * @class   AmListener
 * @ingroup PEARL_base
 * @brief   Base class for listener objects.
 *
 * Base class of Listener objects used by the %AmRuntime
 * class to receive active messages.
 */
/*-------------------------------------------------------------------------*/

class AmListener
    : private pearl::Uncopyable
{
    public:
        /// Reference-counted pointer to a listener object. To ensure
        /// proper deallocation of a listener object, they should only
        /// be handled through these pointer types.
        /// @todo Make AmListener::Ptr an RCSP.
        typedef AmListener* Ptr;


        /// @name Factory methods
        /// @{

        static AmListener::Ptr
        create(const pearl::Communicator& comm);

        /// @}
        /// @name Handling incoming messages
        /// @{

        virtual bool
        probe() = 0;

        /// @}
        /// @name Reference counting interface
        /// @{

        void
        addReference();

        void
        removeReference();

        size_t
        numReferences() const;

        /// @}


    protected:
        /// @name Constructors & destructor
        /// @{

        AmListener(Buffer* buffer);

        virtual
        ~AmListener();

        /// @}
        /// @name Access methods for derived classes
        /// @{

        Buffer&
        getBuffer();

        /// @}


    private:
        /// Associated message buffer
        Buffer* mBuffer;

        /// Count number of references
        size_t mReferences;


        friend class AmRuntime;
};
}    // namespace pearl


#endif    // !PEARL_AMLISTENER_H
