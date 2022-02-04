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


#ifndef PEARL_UNKNOWNLOCATIONGROUP_H
#define PEARL_UNKNOWNLOCATIONGROUP_H


#include <pearl/LocationGroup.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class UnknownLocationGroup.
 *
 *  This header file provides the declaration of the class UnknownLocationGroup
 *  and related functions.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   UnknownLocationGroup
 *  @ingroup PEARL_base
 *  @brief   Representation of a location group definition of unknown type.
 *
 *  Instances of the UnknownLocationGroup class provide information about
 *  location groups of unknown type, e.g., grouping GPU execution threads or
 *  locations storing node-level hardware counters. As concrete
 *  implementation of the LocationGroup interface, UnknownLocationGroup
 *  objects are characterized by an ID, a name, and a parent node (of type
 *  SystemNode) in the system hierarchy, and provide a list of associated
 *  locations.
 **/
/*-------------------------------------------------------------------------*/

class UnknownLocationGroup
    : public LocationGroup
{
    public:
        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Creates a new location group definition instance of unknown type
        /// and initializes it with the given values.
        ///
        /// @param  id      Global location group identifier
        /// @param  name    Group name
        /// @param  parent  Parent node in system tree (non-NULL)
        ///
        UnknownLocationGroup(IdType        id,
                             const String& name,
                             SystemNode*   parent);

        /// @}
        /// @name Access definition data
        /// @{

        /// @brief Get location group type.
        ///
        /// Returns the location group type, encoding more detailed information.
        /// See the documentation of LocationGroup::Type for details. This
        /// subclass will always return LocationGroup::TYPE_UNKNOWN.
        ///
        /// @return %Location group type
        ///
        virtual Type
        getType() const;

        /// @}


    private:
        // --- Private methods & friends -------------------

        virtual std::ostream&
        output(std::ostream& stream) const;
};
}    // namespace pearl


#endif    // !PEARL_UNKNOWNLOCATIONGROUP_H
