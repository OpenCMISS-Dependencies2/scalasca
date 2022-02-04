/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2014                                                **
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
 * @brief   Declaration of the class AmRequestFactory.
 * @ingroup PEARL_base
 *
 *
 * This header file provides the declaration of the class AmRequestFactory.
 */
/*-------------------------------------------------------------------------*/


#ifndef PEARL_AMREQUESTFACTORY_H
#define PEARL_AMREQUESTFACTORY_H


#include <map>

#include <pearl/AmHandler.h>
#include <pearl/AmRequest.h>
#include <pearl/Communicator.h>


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 * @class   AmRequestFactory
 * @ingroup PEARL_base
 * @brief   Request factory for active message request
 *
 * Automatic factory for active message requests.
 */
/*-------------------------------------------------------------------------*/

class AmRequestFactory
{
    public:
        /// @name Type definitions
        /// @{

        /// Listener Callback-function type
        typedef AmRequest::Ptr (* CbFunction)(uint32_t,
                                              const Communicator&,
                                              bool);

        /// Container type for callback lists
        typedef std::map< Paradigm, CbFunction > CbContainer;


        /// @}
        /// @name Factory interface
        /// @{

        static AmRequest::Ptr
        create(uint32_t            remote,
               const Communicator& comm,
               bool                ensureSynchronous = false);

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


#endif    // !PEARL_AMREQUESTFACTORY_H
