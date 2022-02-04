/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2014                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2014                                                     **
**  RWTH Aachen University, JARA-HPC                                       **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SCOUT_AMSYNCPOINTEXCHANGE_H
#define SCOUT_AMSYNCPOINTEXCHANGE_H


#include <map>
#include <queue>
#include <vector>

#include <pearl/AmHandler.h>
#include <pearl/AmRequest.h>
#include <pearl/Event.h>
#include <pearl/RemoteEvent.h>
#include <pearl/pearl_types.h>

#include "scout_types.h"


namespace scout
{
/*-------------------------------------------------------------------------*/
/**
 * @class AmSyncpointExchange
 * @brief Active message handler to exchange Information on Lock Competition.
 *
 * This handler packs all synchpoints, that appeared on the origin process. The target
 * process sends this data to the origin where it is stored.
 */
/*-------------------------------------------------------------------------*/

class AmSyncpointExchange
    : public pearl::AmHandler
{
    public:
        /// @name Singleton interface
        /// @{

        static AmSyncpointExchange&
        getInstance();

        /// @}
        /// @name Packing interface
        /// @{

        static void
        pack(uint32_t        userEvent,
             uint32_t        commId,
             ContentionInfo& info,
             pearl::Buffer&  buffer);

        static void
        pack(uint32_t          userEvent,
             uint32_t          commId,
             ContentionInfo&   info,
             pearl::AmRequest* request);

        /// @}
        /// @name Remote execution
        /// @{

        virtual void
        execute(pearl::Buffer& buffer);

        /// @}


    protected:
        /// @name Constructors & destructors
        /// @{

        AmSyncpointExchange();

        virtual
        ~AmSyncpointExchange();

        /// @}
        /// @name Debugging information
        /// @{

        virtual const std::string&
        getName() const;

        /// @}
};
}    // namespace scout


#endif    // !SCOUT_AMSYNCPOINTEXCHANGE_H
