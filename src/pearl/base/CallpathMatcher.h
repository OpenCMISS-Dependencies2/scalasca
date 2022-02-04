/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2015                                                **
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


#ifndef PEARL_CALLPATHMATCHER_H
#define PEARL_CALLPATHMATCHER_H


#include <functional>

#include <pearl/Callpath.h>
#include <pearl/Callsite.h>
#include <pearl/Region.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    CallpathMatcher.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the functor class CallpathMatcher.
 *
 *  This file provides the declaration of the functor class CallpathMatcher.
 **/
/*-------------------------------------------------------------------------*/


namespace
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   CallpathMatcher
 *  @ingroup PEARL_base
 *  @brief   Functor class to search for a particular callpath.
 *
 *  CallpathMatcher is a functor class which can be used to search for a
 *  particular callpath in a list of Callpath objects. The match criterion
 *  is defined as equality of both the associated source-code region and
 *  the callsite. It can be used in conjunction with STL algorithms such as
 *  find_if().
 *
 *  @code
 *    std::vector< Callpath* > callpaths;
 *
 *    // ... some code working with 'callpaths' ...
 *
 *    std::vector< Callpath* >::iterator it = find_if(callpaths.begin(),
 *                                                    callpaths.end(),
 *                                                    CallpathMatcher(region,
 *                                                                    callsite));
 *  @endcode
 **/
/*-------------------------------------------------------------------------*/

class CallpathMatcher
    : public std::unary_function< const pearl::Callpath*, bool >
{
    public:
        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Initializes the functor object with the given @a region and
        /// @a callsite.
        ///
        /// @param  region    Source-code region to match
        /// @param  callsite  %Callsite to match
        ///
        CallpathMatcher(const pearl::Region&   region,
                        const pearl::Callsite& callsite)
            : mRegion(region),
              mCallsite(callsite)
        {
        }

        /// @}
        /// @name Predicate function
        /// @{

        /// @brief Equality predicate function.
        ///
        /// Returns true if the @a item matches the source-code region and
        /// callsite provided at construction of the functor object, and
        /// false otherwise.
        ///
        /// @param  item  %Callpath used for comparison
        ///
        /// @return True if callpath matches, false otherwise
        ///
        bool
        operator()(const pearl::Callpath* item) const
        {
            return (  (item->getRegion().getId() == mRegion.getId())
                   && (item->getCallsite().getId() == mCallsite.getId()));
        }

        /// @}


    private:
        /// Source-code region to match
        const pearl::Region& mRegion;

        /// %Callsite to match
        const pearl::Callsite& mCallsite;
};
}    // unnamed namespace


#endif    // !PEARL_CALLPATHMATCHER_H
