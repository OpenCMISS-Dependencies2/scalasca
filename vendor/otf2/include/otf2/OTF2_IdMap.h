/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef OTF2_IDMAP_H
#define OTF2_IDMAP_H


/**
 *  @file
 *
 *  @brief Identifier mapping data structure, based on Scalasca's epk_idmap.h
 *
 *  This file provides type definitions and function prototypes for an
 *  identifier mapping data structure which is used to store mapping tables
 *  for converting local into global identifiers.
 *
 *  This mapping data structure can operate in two different modes (see @ref
 *  OTF2_IdMapMode): A dense mapping can be used if the local identifiers
 *  are consecutively enumerated from 0 to N-1. In this case, only the global
 *  identifier are stored in the table at the corresponding entry, leading to
 *  compact storage and fast look-up. By contrast, if the local identifiers
 *  can consist of arbitrary numbers, a sparse mapping is necessary. Here,
 *  (localId, globalId) tuples are stored, which requires a more complicated
 *  look-up procedure.
 */


#include <stddef.h>
#include <stdint.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif


#include <otf2/OTF2_ErrorCodes.h>


/** Opaque data structure representing an ID mapping table. */
typedef struct OTF2_IdMap_struct OTF2_IdMap;


/** Wrapper around enum OTF2_IdMapMode_enum, so that it is guaranteed
 *  that it is a uint8_t */
typedef uint8_t OTF2_IdMapMode;

/**
 * Enumeration type defining the two different modes of an identifier mapping
 * table.
 */
enum OTF2_IdMapMode_enum
{
    OTF2_ID_MAP_DENSE,   /**< Dense mapping table */
    OTF2_ID_MAP_SPARSE   /**< Sparse mapping table */
};


/** @brief Function prototype for use in @a OTF2_IdMap_Traverse.
 */
typedef void
( * OTF2_IdMap_TraverseCallback )( uint64_t localId,
                                   uint64_t globalId,
                                   void*    userData );


#ifdef __cplusplus
extern "C" {
#endif


/**
 *  Creates and returns a new instance of OTF2_IdMap with the given @a mode
 *  and initial @a capacity.  If the memory allocation request cannot be
 *  fulfilled, NULL is returned.
 *
 *  @param mode     Mapping mode.
 *  @param capacity Initial capacity.
 *
 *  @return Pointer to new instance or NULL if memory request couldn't be
 *  fulfilled.
 */
OTF2_IdMap*
OTF2_IdMap_Create( OTF2_IdMapMode mode,
                   uint64_t       capacity );


/**
 *  Creates and returns a new instance of OTF2_IdMap from the array given by
 *  @a mappings.
 *
 *  This creates always a DENSE mapping if @a optimizeSize is false. If it is
 *  true, it creates a SPARSE mapping, if the number of non-identity entries
 *  in the @a mappings array (i.e., mapping[ i ] != i) is less than half the
 *  @a length.
 *
 *  Returns NULL when optimizeSize is true and the number of non-identity
 *  entries equals zero, i.e., the given map is the identity map.
 *
 *  @param length       Number of elements in the @a mappings array.
 *  @param mappings     Array with a dense mapping.
 *  @param optimizeSize Creates a SPARSE mapping, if the number of non-
 *                      identities is less than half the array length.
 *
 *  @return Pointer to new instance or NULL if memory request couldn't be
 *  fulfilled.
 */
OTF2_IdMap*
OTF2_IdMap_CreateFromUint64Array( uint64_t        length,
                                  const uint64_t* mappings,
                                  bool            optimizeSize );


/**
 *  Creates and returns a new instance of OTF2_IdMap from the array given by
 *  @a mappings.
 *
 *  Same as @a OTF2_IdMap_CreateFromUint64Array, except from a uint32_t array.
 *
 *  @param length       Number of elements in the @a mappings array.
 *  @param mappings     Array with a dense mapping.
 *  @param optimizeSize Creates a SPARSE mapping, if the number of non-
 *                      identities is less than half the array length.
 *
 *  @return Pointer to new instance or NULL if memory request couldn't be
 *  fulfilled.
 */
OTF2_IdMap*
OTF2_IdMap_CreateFromUint32Array( uint64_t        length,
                                  const uint32_t* mappings,
                                  bool            optimizeSize );


/**
 *  Destroys the given @a instance of OTF2_IdMap and releases the allocated
 *  memory.
 *
 *  @param instance Object to be freed
 */
void
OTF2_IdMap_Free( OTF2_IdMap* instance );


/**
 *  Returns the actual number of entries stored in the given OTF2_IdMap
 *  @a instance.
 *
 *  @param instance  Queried object.
 *  @param[out] size Number of entries.
 *
 *  @return OTF2_SUCCESS, or error code.
 */
OTF2_ErrorCode
OTF2_IdMap_GetSize( const OTF2_IdMap* instance,
                    uint64_t*         size );


/**
 *  Returns the identifier mapping mode (dense/sparse) used for the given
 *  mapping table @a instance.
 *
 *  @param instance Queried object.
 *  @param[out] mode Identifier mapping mode.
 *
 *  @return OTF2_SUCCESS, or error code.
 */
OTF2_ErrorCode
OTF2_IdMap_GetMode( const OTF2_IdMap* instance,
                    OTF2_IdMapMode*   mode );


/**
 *  Removes all entries in the given mapping table @a instance. It can be
 *  used, e.g., to reuse a mapping table object for new input data.
 *
 *  @param instance Object to remove entries from.
 *
 *  @return OTF2_SUCCESS, or error code.
 */
OTF2_ErrorCode
OTF2_IdMap_Clear( OTF2_IdMap* instance );


/**
 *  Adds the given mapping from @a localId to @a globalId to the mapping
 *  table @a instance. If the current capacity does not suffice, the data
 *  structure is automatically resized.
 *
 *  @note If the mapping table operates in dense mapping mode, the parameter
 *        @a localId has to correspond to the next entry in the mapping table.
 *
 *  @param instance Object to add the mapping to.
 *  @param localId  Local identifier.
 *  @param globalId Global identifier.
 *
 *  @return OTF2_SUCCESS, or error code.
 */
OTF2_ErrorCode
OTF2_IdMap_AddIdPair( OTF2_IdMap* instance,
                      uint64_t    localId,
                      uint64_t    globalId );


/**
 *  Maps the given @a localId to the global id and store it in the storage
 *  provide by @a globalId.
 *
 *  If the given @a localId is not in the mapping, sets globalId to the localId.
 *
 *  @param instance      Object to add the mapping to.
 *  @param localId       Local identifier.
 *  @param[out] globalId Global identifier.
 *
 *  @return OTF2_SUCCESS, or error code.
 */
OTF2_ErrorCode
OTF2_IdMap_GetGlobalId( const OTF2_IdMap* instance,
                        uint64_t          localId,
                        uint64_t*         globalId );


/**
 *  Maps the given @a localId to the global id and store it in the storage
 *  provide by @a globalId.
 *
 *  If the given @a localId is not in the mapping, returns
 *  @eref{OTF2_ERROR_INDEX_OUT_OF_BOUNDS}.
 *
 *  @param instance      Object to add the mapping to.
 *  @param localId       Local identifier.
 *  @param[out] globalId Global identifier.
 *
 *  @return OTF2_SUCCESS, or error code.
 */
OTF2_ErrorCode
OTF2_IdMap_GetGlobalIdSave( const OTF2_IdMap* instance,
                            uint64_t          localId,
                            uint64_t*         globalId );


/**
 *  Calls for each mapping pair the callback @a callback.
 *
 *  @param instance Object to add the mapping to.
 *  @param callback Callback function which is called for each mapping pair.
 *  @param userData Data which is passed to the @a callback function.
 *
 *  @return OTF2_SUCCESS, or error code.
 */
OTF2_ErrorCode
OTF2_IdMap_Traverse( const OTF2_IdMap*           instance,
                     OTF2_IdMap_TraverseCallback callback,
                     void*                       userData );


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OTF2_IDMAP_H */
