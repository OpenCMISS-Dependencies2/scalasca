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


/*-------------------------------------------------------------------------*/
/**
 * @file
 * @brief   Declaration of the class AmListenerFactory.
 * @ingroup PEARL_base
 *
 *
 * This header file provides the declaration of the class AmListenerFactory.
 */
/*-------------------------------------------------------------------------*/


#ifndef PEARL_AMLISTENERFACTORY_H
#define PEARL_AMLISTENERFACTORY_H


#include <map>

#include <pearl/AmListener.h>
#include <pearl/Communicator.h>


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 * @class   AmListenerFactory
 * @ingroup PEARL_base
 * @brief   Listener factory for active message listeners.
 *
 * Automatic factory for active message listeners.
 */
/*-------------------------------------------------------------------------*/

class AmListenerFactory
{
    public:
        /// @name Type definitions
        /// @{

        /// Listener Callback-function type
        typedef AmListener::Ptr (* CbFunction)(const Communicator&);

        /// Container type for callback lists
        typedef std::map< pearl::Paradigm, CbFunction > CbContainer;


        /// @}
        /// @name Factory interface
        /// @{

        static AmListener::Ptr
        create(const pearl::Communicator& comm);

        /// @}
        /// @name Callback interface
        /// @{

        static CbContainer&
        getCallbacks();

        static void
        registerCallback(Paradigm   paradigm,
                         CbFunction callback);

        /// @}
};
}    // namespace pearl


#endif    // !PEARL_AMLISTENERFACTORY_H
