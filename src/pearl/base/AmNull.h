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


#ifndef PEARL_AMNULL_H
#define PEARL_AMNULL_H


#include <string>

#include <pearl/AmHandler.h>


/**
 * @file    AmNull.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class AmNull.
 *
 * This file provides the declaration of the class AmNull. It implements a
 * memory-fence like behavior for the active message runtime.
 */


namespace pearl
{
class AmRequest;
class Buffer;


class AmNull
    : public pearl::AmHandler
{
    public:
        /// @name Singleton interface
        /// @{

        static AmNull&
        getInstance();

        /// @}
        /// @name Packing interface
        /// @{

        static void
        pack(pearl::Buffer& buffer);

        static void
        pack(pearl::AmRequest* request);

        /// @name Remote execution
        /// @{

        virtual void
        execute(pearl::Buffer& buffer);

        /// @}


    protected:
        /// @name Constructors & destructors
        /// @{

        AmNull();

        virtual
        ~AmNull();

        /// @}
        /// @name Debugging information
        /// @{

        virtual const std::string&
        getName() const;

        /// @}
};
}    // namespace pearl


#endif    // !PEARL_AMNULL_H
