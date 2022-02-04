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
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class DefsFactory.
 *
 *  This header file provides the declaration of the class DefsFactory.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_DEFSFACTORY_H
#define PEARL_DEFSFACTORY_H


#include <cassert>
#include <string>
#include <vector>

#include <pearl/CartDimension.h>
#include <pearl/GroupingSet.h>
#include <pearl/Location.h>
#include <pearl/LocationGroup.h>
#include <pearl/Region.h>
#include <pearl/pearl_types.h>


namespace pearl
{
namespace detail
{
// --- Forward declarations -------------------------------------------------

class MutableGlobalDefs;


/*-------------------------------------------------------------------------*/
/**
 *  @class   DefsFactory
 *  @ingroup PEARL_base
 *  @brief   Abstract factory class for global definition objects.
 **/
/*-------------------------------------------------------------------------*/

class DefsFactory
{
    public:
        /// Internal region specifiers
        enum InternalRegionType
        {
            USER_REGION,       ///< User-defined region
            PAUSING_REGION,    ///< %Region where measurement was turned off
            FLUSHING_REGION,   ///< %Buffer flush region
            TASKROOT_REGION,   ///< %Task root region
            THREADROOT_REGION, ///< %Thread root region

            NUMBER_OF_TYPES    ///< Not a valid type, for internal use only
        };


        /// @name Constructors & destructor
        /// @{

        virtual
        ~DefsFactory();

        /// @}
        /// @name Generic factory methods
        /// @{

        void
        createCallingContext(MutableGlobalDefs& defs,
                             uint32_t           id,
                             uint32_t           regionId,
                             uint32_t           sclId,
                             uint32_t           parentId) const;

        void
        createCallpath(MutableGlobalDefs& defs,
                       uint32_t           id,
                       uint32_t           regionId,
                       uint32_t           callsiteId,
                       uint32_t           parentId) const;

        void
        createCallsite(MutableGlobalDefs& defs,
                       uint32_t           id,
                       uint32_t           filenameId,
                       uint32_t           line,
                       uint32_t           regionId) const;

        void
        createCartDimension(MutableGlobalDefs&         defs,
                            uint32_t                   id,
                            uint32_t                   nameId,
                            uint32_t                   size,
                            CartDimension::Periodicity periodicity) const;

        void
        createCartTopology(MutableGlobalDefs& defs,
                           uint32_t           id,
                           uint32_t           nameId,
                           uint32_t           commId,
                           uint8_t            numDimensions,
                           const uint32_t*    dimensionIds) const;

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

        void
        createLocation(MutableGlobalDefs& defs,
                       uint64_t           id,
                       uint32_t           nameId,
                       Location::Type     type,
                       uint64_t           numEvents,
                       uint32_t           parentId) const;

        void
        createLocationGroup(MutableGlobalDefs&  defs,
                            uint32_t            id,
                            uint32_t            nameId,
                            LocationGroup::Type type,
                            uint32_t            parendId) const;

        void
        createRegion(MutableGlobalDefs& defs,
                     uint32_t           id,
                     uint32_t           canonicalNameId,
                     uint32_t           nameId,
                     uint32_t           descriptionId,
                     Region::Role       role,
                     Paradigm           paradigm,
                     uint32_t           filenameId,
                     uint32_t           startLine,
                     uint32_t           endLine,
                     InternalRegionType type) const;

        void
        createSourceLocation(MutableGlobalDefs& defs,
                             uint32_t           id,
                             uint32_t           filenameId,
                             uint32_t           line) const;

        void
        createString(MutableGlobalDefs& defs,
                     uint32_t           id,
                     const std::string& str) const;

        void
        createSystemNode(MutableGlobalDefs& defs,
                         uint32_t           id,
                         uint32_t           nameId,
                         uint32_t           classId,
                         uint32_t           parentId) const;

        void
        createMetric(MutableGlobalDefs& defs,
                     ident_t            id,
                     const std::string& name,
                     const std::string& descr,
                     uint8_t            type,
                     uint8_t            mode,
                     uint8_t            interval) const;

        /// @}
        /// @name MPI-specific factory methods
        /// @{

        virtual void
        createMpiWindow(MutableGlobalDefs& defs,
                        ident_t            id,
                        ident_t            comm_id) const;

        /// @}
        /// @name Singleton interface
        /// @{

        static const DefsFactory*
        instance();

        static void
        registerFactory(const DefsFactory* factory);

        static void
        deregisterFactory();

        /// @}


    private:
        /// Single factory instance
        static const DefsFactory* m_instance;
};


// --- Inline methods -------------------------------------------------------

inline const DefsFactory*
DefsFactory::instance()
{
    assert(m_instance);

    return m_instance;
}
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_DEFSFACTORY_H
