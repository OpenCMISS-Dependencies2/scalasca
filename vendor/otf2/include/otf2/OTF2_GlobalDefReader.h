/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef OTF2_GLOBAL_DEF_READER_H
#define OTF2_GLOBAL_DEF_READER_H


/**
 *  @file
 *
 *  @brief      This is the definition reader.
 */


#include <stddef.h>
#include <stdint.h>


#include <otf2/OTF2_ErrorCodes.h>


#include <otf2/OTF2_Definitions.h>
#include <otf2/OTF2_GlobalDefReaderCallbacks.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief Sets the callback functions for the given reader object. Every time
 *         when OTF2 reads a record, a callback function is called and the records
 *         data is passed to this function. Therefore the programmer needs to set
 *         function pointers at the "callbacks" struct for the record type he wants
 *         to read.
 *
 *  @param reader          This given reader object will be set up with new
 *                         callback functions.
 *  @param callbacks       Struct which holds a function pointer for each
 *                         record type. @eref{OTF2_GlobalDefReaderCallbacks_New}.
 *  @param userData        Data passed as argument @a userData to the record
 *                         callbacks.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_GlobalDefReader_SetCallbacks( OTF2_GlobalDefReader*                reader,
                                   const OTF2_GlobalDefReaderCallbacks* callbacks,
                                   void*                                userData );


/** @brief Reads the given number of records from the global definition reader.
 *
 *  @param reader           The records of this reader will be read when the function is issued.
 *  @param recordsToRead    This variable tells the reader how much records it has to read.
 *  @param[out] recordsRead This is a pointer to variable where the amount of actually read
 *                          records is returned. This may differ to the given recordsToRead if
 *                          there are no more records left in the trace. In this case the programmer
 *                          can easily check that the reader has finished his job by checking
 *                          recordsRead < recordsToRead.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_GlobalDefReader_ReadDefinitions( OTF2_GlobalDefReader* reader,
                                      uint64_t              recordsToRead,
                                      uint64_t*             recordsRead );


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* !OTF2_GLOBAL_DEF_READER_H */
