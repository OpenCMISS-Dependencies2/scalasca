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


#ifndef SCOUT_AMWAITFORPROGRESSREQUEST_H
#define SCOUT_AMWAITFORPROGRESSREQUEST_H


#include <string>

#include <pearl/AmHandler.h>
#include <pearl/RmaEpoch.h>
#include <pearl/pearl_types.h>

#include "scout_types.h"


namespace pearl
{
class AmRequest;
class Buffer;
class Communicator;
}    // namespace pearl


namespace scout
{
/*-------------------------------------------------------------------------*/
/**
 * @class AmWaitForProgressRequest
 * @brief Active message handler for sending an origin-side request for the Wait-for-progress pattern to the target.
 */
/*-------------------------------------------------------------------------*/

class AmWaitForProgressRequest
    : public pearl::AmHandler
{
    public:
        typedef enum
        {
            SINGLE_DEPENDENCY = 0,
            MULTI_DEPENDENCY  = 1
        } dependency_t;


        /// @name Singleton interface
        /// @{

        static AmWaitForProgressRequest&
        getInstance();

        /// @}
        /// @name Handler specific information
        /// @{

        static uint32_t
        generateKey();

        /// @}
        /// @name Packing interface
        /// @{

        static void
        pack(uint32_t                   key,
             const pearl::Communicator& comm,
             dependency_t               dependency,
             const pearl::RmaEpoch&     rmaEpoch,
             pearl::Buffer&             buffer);

        static void
        pack(uint32_t                   key,
             const pearl::Communicator& comm,
             dependency_t               dependency,
             const pearl::RmaEpoch&     rmaEpoch,
             pearl::AmRequest*          request);

        /// @}
        /// @name Remote execution
        /// @{

        virtual void
        execute(pearl::Buffer& buffer);

        /// @}


    protected:
        /// @name Construction & destruction
        /// @{

        AmWaitForProgressRequest();

        virtual
        ~AmWaitForProgressRequest();

        /// @}
        /// @name Debugging information
        /// @{

        virtual const std::string&
        getName() const;

        /// @}


    private:
        /// pointer to external storage for pattern severities
        RemoteSeverityMap* mSeverityMap;

        /// variable to generate unique keys for requests
        static uint32_t mKey;
};
}    // namespace scout


#endif    // !SCOUT_AMWAITFORPROGRESSREQUEST_H
