/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class LocalIdMaps.
 *
 *  This file provides the declaration of the class LocalIdMaps.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_LOCALIDMAPS_H
#define PEARL_LOCALIDMAPS_H


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @ingroup PEARL_base
 *  @brief   Base class for format-specific local-to-global definition
 *           identifier mappings.
 *
 *  The LocalIdMaps class provides the common interface of derived,
 *  format-specific local-to-global definition identifier mapping classes.
 *  It is intentionally not defined as an abstract base class, to allow for
 *  using instances of LocalIdMaps in case a trace format does not require
 *  or support identifier mappings.
 **/
/*-------------------------------------------------------------------------*/

class LocalIdMaps
{
    public:
        // --- Public member functions ---------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Default constructor.
        ///
        /// Creates a new LocalIdMaps instance.
        ///
        LocalIdMaps();

        /// @brief Destructor.
        ///
        /// Destroys the LocalIdMaps instance.
        ///
        virtual
        ~LocalIdMaps();

        /// @}
};
}    // namespace pearl


#endif    // !PEARL_LOCALIDMAPS_H
