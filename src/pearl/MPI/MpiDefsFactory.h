/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
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
 *  @file
 *  @ingroup PEARL_mpi
 *  @brief   Declaration of the class MpiDefsFactory.
 *
 *  This header file provides the declaration of the class MpiDefsFactory.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_MPIDEFSFACTORY_H
#define PEARL_MPIDEFSFACTORY_H


#include "DefsFactory.h"


namespace pearl
{
namespace detail
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   MpiDefsFactory
 *  @ingroup PEARL_mpi
 *  @brief   Abstract factory class for MPI-specific global definition
 *           objects.
 **/
/*-------------------------------------------------------------------------*/

class MpiDefsFactory
    : public DefsFactory
{
    public:
        /// @name Generic factory methods
        /// @{

        virtual void
        createCommunicator(MutableGlobalDefs& defs,
                           uint32_t           id,
                           uint32_t           nameId,
                           uint32_t           commSetId,
                           uint32_t           parentId) const;

        virtual void
        createGroupingSet(MutableGlobalDefs&             defs,
                          uint32_t                       id,
                          uint32_t                       nameId,
                          GroupingSet::SetType           type,
                          Paradigm                       paradigm,
                          const GroupingSet::Properties& properties,
                          uint32_t                       numMembers,
                          const uint64_t*                members) const;

        /// @}
        /// @name MPI-specific factory methods
        /// @{

        virtual void
        createMpiWindow(MutableGlobalDefs& defs,
                        ident_t            id,
                        ident_t            comm_id) const;

        /// @}
};
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_MPIDEFSFACTORY_H
