/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


#ifndef OTF2_GLOBAL_DEF_WRITER_H
#define OTF2_GLOBAL_DEF_WRITER_H


/**
 *  @file
 *  @source     templates/OTF2_GlobalDefWriter.tmpl.h
 *
 *  @brief      This layer always writes globally defined OTF2 definition
 *              records and is used to write either the global definitions
 *              in addition to local definitions or write all definitions as
 *              globally valid in combination with OTF2_GlobalEventWriter.
 *              Global definitions are stored in one global definition file,
 *              which makes it nearly impossible to write them in a distributed
 *              manner. It is therefore only allowed to get such a writer from
 *              an OTF2_Archive which is the master in the collective context.
 */


#include <stdint.h>


#include <otf2/OTF2_ErrorCodes.h>


#include <otf2/OTF2_GeneralDefinitions.h>
#include <otf2/OTF2_AttributeValue.h>
#include <otf2/OTF2_Definitions.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief Typedef of the struct which keeps all necessary information of
           a global definition writer. Can be used to reference these structs
           from external
 */
typedef struct OTF2_GlobalDefWriter_struct OTF2_GlobalDefWriter;


/** @brief Returns the current number of written definitions of a global
 *         definition writer.
 *
 *  @param writerHandle             Handle to the global definition writer.
 *  @param[out] numberOfDefinitions Storage for the number of definitions.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_GlobalDefWriter_GetNumberOfDefinitions( OTF2_GlobalDefWriter* writerHandle,
                                             uint64_t*             numberOfDefinitions );


/** @brief Returns the current number of written location definitions
 *         of a global definition writer.
 *
 *  @param writerHandle           Handle to the global definition writer.
 *  @param[out] numberOfLocations Storage for the number of locations.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_GlobalDefWriter_GetNumberOfLocations( OTF2_GlobalDefWriter* writerHandle,
                                           uint64_t*             numberOfLocations );

@otf2 for def in defs|global_defs:

/** @brief Writes a @eref{@@def.name@@} definition record into the GlobalDefWriter.
 *
 *  @@def.doxygendoc()@@
 *  @@def.doxygenargs(prepend=[('writerHandle', 'The writer handle.')])@@
 *
 *  @since Version @@def.since@@
 *
@otf2 if def.deprecated:
 *  @deprecated In version @@def.deprecated@@
 *
@otf2 endif
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_GlobalDefWriter_Write@@def.name@@( OTF2_GlobalDefWriter* writerHandle @@def.funcargs()@@ );

@otf2 endfor

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* !OTF2_GLOBAL_DEF_WRITER_H */
