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


#ifndef SCOUT_AMWAITFORPROGRESSRESPONSE_H
#define SCOUT_AMWAITFORPROGRESSRESPONSE_H


#include <string>

#include <pearl/AmHandler.h>
#include <pearl/AmRequest.h>
#include <pearl/Event.h>
#include <pearl/RmaEpoch.h>
#include <pearl/pearl_types.h>

#include "LockEpochQueue.h"


namespace pearl
{
class Communicator;
}    // namespace pearl


namespace scout
{
/*-------------------------------------------------------------------------*/
/**
 * @class AmWaitForProgressResponse
 * @brief Active message handler for target-side responses to the origin for the Wait-for-progress pattern.
 *
 * Active message handler for responses on Wait-for-progress requests
 */
/*-------------------------------------------------------------------------*/

class AmWaitForProgressResponse
    : public pearl::AmHandler
{
    public:
        /// @name Singleton interface
        /// @{

        static AmWaitForProgressResponse&
        getInstance();

        /// @}
        /// @name Packing interface
        /// @{

        static void
        pack(uint32_t               key,
             uint32_t               local_rank,
             uint32_t               event_id,
             uint32_t               comm_id,
             const pearl::RmaEpoch& epoch,
             pearl::Buffer&         buffer);

        static void
        pack(uint32_t               key,
             uint32_t               local_rank,
             uint32_t               event_id,
             uint32_t               comm_id,
             const pearl::RmaEpoch& epoch,
             pearl::AmRequest*      request);

        /// @}
        /// @name Remote execution
        /// @{

        virtual void
        execute(pearl::Buffer& buffer);

        /// @}


    protected:
        /// @name Construction & destruction
        /// @{

        AmWaitForProgressResponse();

        virtual
        ~AmWaitForProgressResponse();

        /// @}
        /// @name Debugging information
        /// @{

        virtual const std::string&
        getName() const;

        /// @}
};
}    // namespace scout


#endif    // !SCOUT_AMWAITFORPROGRESSRESPONSE_H
