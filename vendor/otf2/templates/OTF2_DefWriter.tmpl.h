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


#ifndef OTF2_DEF_WRITER_H
#define OTF2_DEF_WRITER_H


/**
 *  @file
 *  @source     templates/OTF2_DefWriter.tmpl.h
 *
 *  @brief      This file provides all routines that write definition records
 *              of a single location.
 */


#include <stdint.h>


#include <otf2/OTF2_ErrorCodes.h>


#include <otf2/OTF2_GeneralDefinitions.h>
#include <otf2/OTF2_AttributeValue.h>
#include <otf2/OTF2_Definitions.h>
#include <otf2/OTF2_IdMap.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief Handle definition for the external definition writer. */
typedef struct OTF2_DefWriter_struct OTF2_DefWriter;


/** @brief
 *  Returns the location ID of the location which is related to
 *  the writer object.
 *
 *  @param writer   Writer object.
 *  @param location Return location reference.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_GetLocationID( const OTF2_DefWriter* writer,
                              OTF2_LocationRef*     location );

@otf2 for def in defs|local_defs:

/** @brief Writes a @@def.name@@ definition record into the DefWriter.
 *
 *  @@def.doxygendoc()@@
 *  @@def.doxygenargs(prepend=[('writer', 'Writer object.')])@@
 *
 *  @since Version @@def.since@@
@otf2 if def.deprecated:
 *
 *  @deprecated In version @@def.deprecated@@
@otf2 endif
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode
OTF2_DefWriter_Write@@def.name@@( OTF2_DefWriter* writer @@def.funcargs()@@ );

@otf2 endfor

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* !OTF2_DEF_WRITER_H */
