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


#ifndef SCOUT_AMSEVERITYEXCHANGE_H
#define SCOUT_AMSEVERITYEXCHANGE_H


#include <string>

#include <pearl/AmHandler.h>
#include <pearl/pearl_types.h>


// forward declarations
namespace pearl
{
class AmRequest;
class Buffer;
class CallbackManager;
}    // namespace pearl


namespace scout
{
/*-------------------------------------------------------------------------*/
/**
 * @class AmSeverityExchange
 *
 * Class to exchange remotely stored severities to the location they
 * have to be saved on.
 */
/*-------------------------------------------------------------------------*/

class AmSeverityExchange
    : public pearl::AmHandler
{
    public:
        /// @name Singleton interface
        /// @{

        static AmSeverityExchange&
        getInstance();

        /// @}
        /// @name Packing interface
        /// @{

        static void
        pack(uint32_t           userEvent,
             pearl::ident_t     eventId,
             pearl::ident_t     cnodeId,
             uint32_t           rank,
             pearl::timestamp_t severity,
             pearl::Buffer&     buffer);

        static void
        pack(uint32_t           userEvent,
             pearl::ident_t     eventId,
             pearl::ident_t     cnodeId,
             uint32_t           rank,
             pearl::timestamp_t severity,
             pearl::AmRequest*  request);

        /// @}
        /// @name Remote execution
        /// @{

        virtual void
        execute(pearl::Buffer& buffer);

        /// @}


    protected:
        /// @name Construction & destruction
        /// @{

        AmSeverityExchange();

        virtual
        ~AmSeverityExchange();

        /// @}
        /// @name Debuffing information
        /// @{

        virtual const std::string&
        getName() const;

        /// @}
};
}    // namespace scout


#endif    // !SCOUT_AMSEVERITYEXCHANGE_H
