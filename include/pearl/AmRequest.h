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


#ifndef PEARL_AMREQUEST_H
#define PEARL_AMREQUEST_H


#include <inttypes.h>

#include <memory>

#include <pearl/Uncopyable.h>


/*-------------------------------------------------------------------------*/
/**
 * @file    AmRequest.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class AmRequest.
 *
 * This header files provides the declaration of the class
 * AmRequest. This is the abstract base class used by the
 * runtime.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{
// Forward declarations
class Buffer;
class AmRequest;
class Communicator;


/*-------------------------------------------------------------------------*/
/**
 * @class   AmRequest
 * @ingroup PEARL_base
 * @brief   Base class for active message requests
 *
 * This is the abstract base class defining the method interface derived
 * classes can be handled with by the %AmRuntime object.
 *
 * Different requests (derived request objects) will use a buffer object
 * which is derived from %Buffer to enable the communication.
 */
/*-------------------------------------------------------------------------*/

class AmRequest
    : private pearl::Uncopyable
{
    public:
        /// Smartpointer to an %AmRequest object to completely transfer
        /// ownership to the %AmRuntime object
        typedef std::auto_ptr< AmRequest > Ptr;

        /// Callback Datatype for factory callback
        struct CbData
        {
            CbData(uint32_t            remote,
                   const Communicator& comm,
                   bool                ensureSynchronous = false)
                : mRemote(remote),
                  mComm(comm),
                  mEnsureSynchronous(ensureSynchronous)
            {
            }

            uint32_t            mRemote;
            const Communicator& mComm;
            bool                mEnsureSynchronous;
        };


        /// @name Constructors & destructors
        /// @{

        AmRequest(Buffer*             buffer,
                  uint32_t            remote,
                  const Communicator& comm,
                  bool                ensureSynchronous);

        virtual
        ~AmRequest();

        /// @}
        /// @name Handling communication
        /// @{

        virtual void
        start() = 0;

        virtual bool
        test() = 0;

        virtual void
        cancel() = 0;

        bool
        ensureSynchronous() const;

        /// @}
        /// @name Access routines for derived classes
        /// @{

        Buffer*
        getBuffer();

        uint32_t
        getRemote() const;

        const Communicator&
        getComm() const;

        /// @}


    private:
        /// Associated message buffer
        Buffer* mBuffer;

        /// Remote process identifier
        uint32_t mRemote;

        /// Associated communicator
        const Communicator& mComm;

        /// Flag whether synchronous messaging should be enforced
        bool mEnsureSynchronous;
};


bool
isFinished(AmRequest* request);
}    // namespace pearl


#endif    // !PEARL_AMREQUEST_H
