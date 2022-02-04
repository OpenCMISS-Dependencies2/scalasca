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


#ifndef PEARL_MPIAMREQUEST_H
#define PEARL_MPIAMREQUEST_H


#include <inttypes.h>

#include <pearl/AmRequest.h>


/*-------------------------------------------------------------------------*/
/**
 * @file    AmRequest.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class MpiAmRequest.
 *
 * This header files provides the declaration of the class
 * MpiAmRequest.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{
class Communicator;
class MpiComm;


/*-------------------------------------------------------------------------*/
/**
 * @class   MpiAmRequest
 * @ingroup PEARL_base
 * @brief   Active message request using an MpiMessage buffer.
 *
 * An active message request class using an %MpiMessage buffer.
 */
/*-------------------------------------------------------------------------*/

class MpiAmRequest
    : public AmRequest
{
    public:
        /// @name Constructors & destructors
        /// @{

        static AmRequest::Ptr
        create(uint32_t            remote,
               const Communicator& comm,
               bool                ensureSynchronous = false);

        virtual
        ~MpiAmRequest();

        /// @}
        /// @name Handling requests on origin side
        /// @{

        virtual void
        start();

        virtual bool
        test();

        virtual void
        cancel();

        /// @}


    private:
        MpiAmRequest(uint32_t       remote,
                     const MpiComm& comm,
                     bool           ensureSynchronous);
};
}    // namespace pearl


#endif    // !PEARL_MPIAMREQUEST_H
