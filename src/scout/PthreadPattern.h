/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2019                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SCOUT_PTHREADPATTERN_H
#define SCOUT_PTHREADPATTERN_H


#include "Pattern.h"


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @brief Declaration of the PthreadPattern class.
 *
 *  This header file provides the declaration of the PthreadPattern class
 *  which serves as an abstract base class for all Pthread-specific
 *  performance properties calculated by SCOUT.
 **/
/*-------------------------------------------------------------------------*/


namespace scout
{
/*-------------------------------------------------------------------------*/
/**
 *  @class PthreadPattern
 *  @brief Abstract base class for all Pthread-specific properties calculated
 *         by SCOUT.
 *
 *  This class serves as an abstract base class for all Pthread-specific
 *  performance properties calculated by SCOUT.
 **/
/*-------------------------------------------------------------------------*/

class PthreadPattern
    : public Pattern
{
    protected:
        /// @name Data collation control
        /// @{

        /// @brief Controls collation of data for the given call-tree node.
        ///
        /// This method controls the collation of severity data for the given
        /// call path @p cnode. The method returns @c true for all non-Pthread
        /// call paths since it is guaranteed that no Pthread-related severity
        /// data exists for them.  For all other call paths, it returns
        /// @c false.
        ///
        /// @param cnode
        ///     Current call path
        /// @returns
        ///     Returns @c true if @p cnode is a Pthread call path, @c false
        ///     otherwise
        virtual bool
        skip_cnode(const pearl::Callpath& cnode) const;

        /// @}
};
}    // namespace scout


#endif    // !SCOUT_PTHREADPATTERN_H
