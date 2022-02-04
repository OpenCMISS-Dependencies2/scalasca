/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
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


#ifndef PEARL_AMHANDLER_H
#define PEARL_AMHANDLER_H


#include <inttypes.h>

#include <string>

#include <pearl/Uncopyable.h>


/*-------------------------------------------------------------------------*/
/**
 * @file    AmHandler.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class AmHandler.
 *
 * This header files provides the declaration of the class
 * AmHandler. This is the abstract base class for active
 * message handlers used by the runtime.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{
// Forward declarations
class AmRuntime;
class Buffer;
class CallbackData;


/*-------------------------------------------------------------------------*/
/**
 * @class   AmHandler
 * @ingroup PEARL_base
 * @brief   Base class for active message handlers
 *
 * This is the base class for active message handlers. Only handlers
 * derived of this base class can be registered with the AmRuntime
 * object.
 */
/*-------------------------------------------------------------------------*/

class AmHandler
    : private pearl::Uncopyable
{
    public:
        /// Identifier for unregistered handlers
        static const uint32_t UNREGISTERED;


        /// @name Query functions
        /// @{

        uint32_t
        getId();

        /// @}
        /// @name Target-side functionality
        /// @{

        virtual void
        execute(Buffer& buffer) = 0;

        /// @}


    protected:
        /// @name Constructors & destructor
        /// @{

        AmHandler();

        virtual
        ~AmHandler();

        /// @}
        /// @name External interfaces
        /// @{

        void
        setId(uint32_t id);

        /// @}
        /// @name Debugging information
        /// @{

        virtual const std::string&
        getName() const = 0;

        /// @}


    private:
        /// Id this handler is registered under with the runtime
        uint32_t mId;


        // AmRuntime needs to be a friend to set the handler ID during
        // registration.
        friend class AmRuntime;
};
}    // namespace pearl


#endif    // !PEARL_AMHANDLER_H
