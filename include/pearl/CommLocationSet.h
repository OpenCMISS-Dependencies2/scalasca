/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2017-2018                                                **
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
 *  @brief   Declaration of the class CommLocationSet.
 *
 *  This header file provides the declaration of the class CommLocationSet.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_COMMLOCATIONSET_H
#define PEARL_COMMLOCATIONSET_H


#include <pearl/LocationSet.h>
#include <pearl/Paradigm.h>


namespace pearl
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   CommLocationSet
 *  @ingroup PEARL_base
 *  @brief   Grouping set definition representation for a set of Location
 *           definition objects participating in a particular paradigm.
 *
 *  The CommLocationSet class provides a specialized location set definition
 *  representation for an ordered set of Location definition objects which
 *  participate in a particular paradigm.  Note that a CommLocationSet does
 *  **not** take ownership of the provided Location definition objects.
 *  Thus, care has to be taken with respect to the deletion and lifetime of
 *  the referenced Location objects.
 **/
/*-------------------------------------------------------------------------*/

class CommLocationSet
    : public pearl::LocationSet
{
    public:
        // --- Static class data ---------------------------

        /// Numerical identifier used for undefined communication location
        /// set definitions.
        static const IdType NO_ID = (~static_cast< IdType >(0));

        /// Static instance of class CommLocationSet representing undefined
        /// communication location set entries in other definitions.
        static const CommLocationSet UNDEFINED;


        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Constructor.
        ///
        /// Constructs a new instance with the given global identifier @p id,
        /// communication location set @p name, the corresponding @p paradigm,
        /// and a @p members vector providing an ordered list of Location
        /// definition objects defining the set.
        ///
        /// @param id
        ///     Global grouping set identifier
        /// @param name
        ///     Communication location set name
        /// @param paradigm
        ///     Communication paradigm
        /// @param members
        ///     List of Location definition objects defining the set
        ///
        /// @exception_strong
        ///
        /// @throws std::bad_alloc
        ///     on failure to allocate storage
        ///
        CommLocationSet(IdType                                id,
                        const String&                         name,
                        Paradigm                              paradigm,
                        const std::vector< const Location* >& members);

        /// @}
        /// @name Query functions
        /// @{

        /// @brief Get grouping set type.
        ///
        /// Returns the type of the grouping set.
        ///
        /// @returns
        ///     Set type (i.e., GroupingSet::COMM_LOCATION_SET)
        ///
        /// @exception_nothrow
        ///
        virtual SetType
        getType() const;

        /// @brief Get paradigm.
        ///
        /// Returns the paradigm associated to this communication location set.
        ///
        /// @returns
        ///     Communication paradigm
        ///
        /// @exception_nothrow
        ///
        Paradigm
        getParadigm() const;

        /// @}


    private:
        // --- Data members --------------------------------

        /// Communication paradigm
        Paradigm mParadigm;


        // --- Private methods -----------------------------

        /// @name Stream I/O functions
        /// @{

        /// @brief Stream output helper function.
        ///
        /// Prints the communication location set's attributes to the given
        /// output @p stream.
        ///
        /// @param stream
        ///     Output stream
        /// @returns
        ///     Output stream
        ///
        virtual std::ostream&
        output(std::ostream& stream) const;

        /// @}
};
}    // namespace pearl


#endif    // !PEARL_COMMLOCATIONSET_H
