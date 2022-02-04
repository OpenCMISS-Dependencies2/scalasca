/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
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


#ifndef PEARL_CALLTREE_H
#define PEARL_CALLTREE_H


#include <stdint.h>

#include <iosfwd>
#include <vector>

#include <pearl/Callpath.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    CallTree.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class CallTree.
 *
 *  This header file provides the declaration of the class CallTree and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class Buffer;
class Callsite;
class GlobalDefs;
class Region;


namespace detail
{
/*-------------------------------------------------------------------------*/
/**
 *  @class   CallTree
 *  @ingroup PEARL_base
 *  @brief   Container class for call trees.
 *
 *  Instances of the CallTree class store the data of either process-local
 *  or global call trees.  Each call tree consists of a set of Callpath
 *  objects encoding the actual parent-child relationships.
 **/
/*-------------------------------------------------------------------------*/

class CallTree
{
    public:
        // --- Public methods ------------------------------

        /// @name Constructors & destructor
        /// @{

        /// @brief Default constructor.
        ///
        /// Creates a new instance representing an empty call tree.
        ///
        CallTree();

        /// @brief Deserialization constructor.
        ///
        /// Creates a new instance and initializes it with the call tree data
        /// extracted from the given memory @a buffer object.
        ///
        /// @param  defs    Global definition object
        /// @param  buffer  Memory buffer object
        ///
        CallTree(const GlobalDefs& defs,
                 Buffer&           buffer);

        /// @brief Destructor.
        ///
        /// Releases all allocated resources and destroys the instance.
        ///
        ~CallTree();

        /// @}
        /// @name Query/modify the call tree information
        /// @{

        /// @brief Get number of callpaths.
        ///
        /// Returns the total number of callpaths currently stored in the
        /// call tree.
        ///
        /// @return Number of callpaths
        ///
        uint32_t
        numCallpaths() const;

        /// @brief Get callpath by ID.
        ///
        /// Returns a pointer to the callpath object stored with the given
        /// identifier @a id in the range [0,@#callpaths-1], or NULL if
        /// @a id equals to Callpath::NO_ID.
        ///
        /// @param  id  %Callpath identifier
        ///
        /// @return Pointer to requested callpath or NULL
        ///
        Callpath*
        getCallpath(Callpath::IdType id) const;

        /// @brief Get callpath by properties.
        ///
        /// Returns a pointer to the callpath object referencing the given
        /// @a region and @a callsite of the @a parent callpath (NULL for root
        /// callpaths). If such a callpath does not exist, it is automatically
        /// created.
        ///
        /// @param  region    Called region
        /// @param  callsite  Specific callsite (Callsite::UNDEFINED if not
        ///                   applicable)
        /// @param  parent    Parent callpath (NULL for root callpaths)
        ///
        /// @return Pointer to (potentially newly created) callpath object
        ///
        Callpath*
        getCallpath(const Region&   region,
                    const Callsite& callsite,
                    Callpath*       parent);

        /// @brief Add callpath object.
        ///
        /// Adds the given @a callpath to the call tree. Valid arguments are
        /// only non-NULL pointers to callpath objects with subsequent IDs,
        /// i.e., the identifier of @a callpath has to match the value returned
        /// by numCallpaths() before the call.
        ///
        /// @param  callpath  %Callpath to be added
        ///
        void
        addCallpath(Callpath* callpath);

        /// @}
        /// @name Merging of call trees
        /// @{

        /// @brief Merge another call tree.
        ///
        /// Merges the given @a calltree into the tree represented by the
        /// instance. Callpaths which only exist in the merged tree will be
        /// automatically added, all others will be fused. This implies that
        /// the callpath identifiers used in the given @a calltree are
        /// typically no longer valid in the merged call tree.
        ///
        /// @param  calltree  %CallTree to be merged
        ///
        void
        merge(const CallTree& calltree);

        /// @}
        /// @name Serialize call tree data
        /// @{

        /// @brief Serialize call tree into memory buffer.
        ///
        /// Serializes the call tree data into the provided memory @a buffer.
        /// Call trees can then be reconstructed using the deserialization
        /// constructor CallTree(const GlobalDefs& defs, Buffer& buffer).
        ///
        /// @param  buffer  Memory buffer object
        ///
        void
        pack(Buffer& buffer) const;

        /// @}
        /// @name Tracking modifications
        /// @{

        bool
        getModified() const
        {
            return mModified;
        }

        void
        setModified(bool flag = true)
        {
            mModified = flag;
        }

        /// @}


    private:
        // --- Data members --------------------------------

        /// Container type for callpaths
        typedef std::vector< Callpath* > CallpathContainer;


        /// Callpath definitions: callpath ID |-@> callpath
        CallpathContainer mCallpaths;

        /// List of root callpaths
        CallpathContainer mRootCallpaths;

        /// Denotes whether the call tree has been modified after the last
        /// reset of this flag
        bool mModified;


        // --- Private methods & friends -------------------

        void
        clear();

        std::ostream&
        print(std::ostream& stream) const;

        Callpath*
        findCallpath(const Region&   region,
                     const Callsite& callsite,
                     Callpath*       parent) const;


        friend std::ostream&
        operator<<(std::ostream&   stream,
                   const CallTree& item);
};


// --- Related functions ----------------------------------------------------

/// @name Stream I/O functions
/// @{

/// @brief   Stream output operator.
/// @relates CallTree
///
/// Prints the contents of the given call tree @a item to an output @a stream
/// in a human-readable form.
///
/// @param  stream  Output stream
/// @param  item    %CallTree object
///
/// @return Output stream
///
std::ostream&
operator<<(std::ostream&   stream,
           const CallTree& item);

/// @}
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_CALLTREE_H
