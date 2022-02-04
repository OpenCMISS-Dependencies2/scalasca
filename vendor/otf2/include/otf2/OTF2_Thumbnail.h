/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */


#ifndef OTF2_THUMBNAIL_H
#define OTF2_THUMBNAIL_H


/**
 *  @file
 *
 *  @brief      This lowest user-visible layer provides write routines to
 *              read and write thumbnail data.
 */


#include <stdint.h>


#include <otf2/OTF2_GeneralDefinitions.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief Keeps all necessary information about the thumb writer.
 *  See OTF2_ThumbWriter_struct for detailed information. */
typedef struct OTF2_ThumbWriter_struct OTF2_ThumbWriter;


/** @brief Keeps all necessary information about the event reader.
 *  See OTF2_ThumbReader_struct for detailed information. */
typedef struct OTF2_ThumbReader_struct OTF2_ThumbReader;


/** @brief Reads a thumbnail header.
 *
 *  A thumbnail header contains some meta information for a thumbnail.
 *
 *  @param reader                Reader object.
 *  @param[out] name             Name of thumbnail. Allocated with @a malloc.
 *  @param[out] description      Description of thumbnail. Allocated with @a malloc.
 *  @param[out] type             Type of thumbnail.
 *  @param[out] numberOfSamples  Number of samples.
 *  @param[out] numberOfMetrics  Number of metrics.
 *  @param[out] refsToDefs       The sorted set of references to definitions
 *                               used in a thumbnail sample.
 *                               Allocated with @a malloc.
 *                               Contains numberOfMetrics entries
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_ThumbReader_GetHeader( OTF2_ThumbReader*   reader,
                            char** const        name,
                            char** const        description,
                            OTF2_ThumbnailType* type,
                            uint32_t*           numberOfSamples,
                            uint32_t*           numberOfMetrics,
                            uint64_t**          refsToDefs );


/** @brief Reads a thumbnail sample.
 *
 *  @param reader                Reader object.
 *  @param[out] baseline         Baseline for this sample. If zero, the baseline
 *                               is the sum of all metric values in this sample.
 *  @param numberOfMetrics       Number of metric sample values.
 *  @param[out] metricSamples    Metric sample values.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_ThumbReader_ReadSample( OTF2_ThumbReader* reader,
                             uint64_t*         baseline,
                             uint32_t          numberOfMetrics,
                             uint64_t*         metricSamples );


/** @brief Writes a thumbnail sample.
 *
 *  @param writer           Writer object.
 *  @param baseline         Baseline for this sample. If zero, the baseline is
 *                          the sum of all metric values in this sample.
 *  @param numberOfMetrics  Number of metric sample values.
 *  @param metricSamples    Metric sample values.
 *
 *  @since Version 1.2
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_ThumbWriter_WriteSample( OTF2_ThumbWriter* writer,
                              uint64_t          baseline,
                              uint32_t          numberOfMetrics,
                              const uint64_t*   metricSamples );


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* !OTF2_THUMBNAIL_H */
