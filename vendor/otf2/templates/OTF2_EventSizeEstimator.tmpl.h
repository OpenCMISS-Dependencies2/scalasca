/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef OTF2_EVENT_SIZE_ESTIMATOR_H
#define OTF2_EVENT_SIZE_ESTIMATOR_H


/**
 *  @file
 *  @source     templates/OTF2_EventSizeEstimator.tmpl.h
 *
 *  @brief      Provides a interface to estimate the size of a resulting trace
 *              file.
 */


#include <stdint.h>
#include <stdlib.h>


#include <otf2/OTF2_ErrorCodes.h>


#include <otf2/OTF2_AttributeList.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief Keeps all necessary information about the event size estimator.
 *  See OTF2_EventSizeEstimator_struct for detailed information. */
typedef struct OTF2_EventSizeEstimator OTF2_EventSizeEstimator;


/** @brief Creates a new OTF2_EventSizeEstimator object.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_EventSizeEstimator*
OTF2_EventSizeEstimator_New( void );


/** @brief Deletes an OTF2_EventSizeEstimator object.
 *
 *  @param estimator   Estimator object.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EventSizeEstimator_Delete( OTF2_EventSizeEstimator* estimator );

@otf2 for entry in MappingType.entries:

/** @brief Sets the number of @@entry.def.ref_name@@ definitions used.
 *
 * Definition ids are considered to be in the range
 * [0, numberOf@@entry.def.ref_name@@Definitions).
 *
 *  @param estimator   Estimator object.
 *  @param numberOf@@entry.def.ref_name@@Definitions The number of definitions.
 *
 *  @since Version @@entry.def.since@@
@otf2  if entry.def.deprecated:
 *
 *  @deprecated In version @@entry.def.deprecated@@
@otf2  endif
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_EventSizeEstimator_SetNumberOf@@entry.def.ref_name@@Definitions( OTF2_EventSizeEstimator* estimator,
                                                                      @@entry.def.self_attr.type.c_orig@@ numberOf@@entry.def.ref_name@@Definitions );

@otf2 endfor

/** @brief Returns an estimate for the size of the definition chunk size.
 *
 *  As definition records like the @eref{Group} definition may break the
 *  chunk size, this gives an upper bound for a chunksize, so that all
 *  individual definition records fit into one chunk. Note, this estimate
 *  does not include @eref{String} definitions, which may have an arbitrary
 *  length.
 *
 *  @param estimator   Estimator object.
 *
 *  @return The estimated definition chunk size, rounded-up to a multiple of
 *          @eref{OTF2_CHUNK_SIZE_MIN}. Or 0 if the estimate is larger than
 *          @eref{OTF2_CHUNK_SIZE_MAX}.
 */
size_t
OTF2_EventSizeEstimator_GetDefChunkSize( OTF2_EventSizeEstimator* estimator );


/** @brief Returns the size for a timestamp record.
 *
 *  OTF2 does only store a timestamp, if it changed between two events.
 *
 *  @param estimator   Estimator object.
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfTimestamp( OTF2_EventSizeEstimator* estimator );


/** @brief Returns the size estimate for an attribute list.
 *
 *  The attribute list should be filled with the used types. The attribute
 *  references are taken from the number of attribute definitions and the values
 *  are the upper bounds for integral and floating point types, and the
 *  estimates for definition references.
 *
 *  @param estimator      Estimator object.
 *  @param attributeList  Attribute List.
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOfAttributeList( const OTF2_EventSizeEstimator* estimator,
                                                const OTF2_AttributeList*      attributeList );

@otf2 for event in events:

/** @brief Calculates the size estimate for the @@event.name@@ event.
 *  @@event.doxygenargs(attributes=event.array_attributes,
                        resolve_array_attributes=False,
                        prepend=[('estimator', 'Estimator object.')])@@
 *
 *  @since Version @@event.since@@
@otf2  if event.deprecated:
 *
 *  @deprecated In version @@event.deprecated@@
@otf2  endif
 *
 *  @return The estimated size.
 */
size_t
OTF2_EventSizeEstimator_GetSizeOf@@event.name@@Event( OTF2_EventSizeEstimator* estimator@@event.funcargs(attributes=event.array_attributes, resolve_array_attributes=False)@@ );

@otf2 endfor

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* !OTF2_EVENT_SIZE_ESTIMATOR_H */
