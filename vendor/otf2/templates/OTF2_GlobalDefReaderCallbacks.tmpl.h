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


#ifndef OTF2_GLOBAL_DEF_READER_CALLBACKS_H
#define OTF2_GLOBAL_DEF_READER_CALLBACKS_H


/**
 *  @file
 *  @source     templates/OTF2_GlobalDefReaderCallbacks.tmpl.h
 *
 *  @brief      This defines the callbacks for the global definition reader.
 */


#include <stdint.h>


#include <otf2/OTF2_ErrorCodes.h>


#include <otf2/OTF2_GeneralDefinitions.h>
#include <otf2/OTF2_AttributeValue.h>
#include <otf2/OTF2_Definitions.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief Opaque struct which holds all global definition record callbacks.
 */
typedef struct OTF2_GlobalDefReaderCallbacks_struct OTF2_GlobalDefReaderCallbacks;


/** @brief Allocates a new struct for the global definition callbacks.
 *
 *  @return A newly allocated struct of type @eref{OTF2_GlobalDefReaderCallbacks}.
 */
OTF2_GlobalDefReaderCallbacks*
OTF2_GlobalDefReaderCallbacks_New( void );


/** @brief Deallocates a struct for the global definition callbacks.
 *
 *  @param globalDefReaderCallbacks Handle to a struct previously allocated
 *                                  with @eref{OTF2_GlobalDefReaderCallbacks_New}.
 */
void
OTF2_GlobalDefReaderCallbacks_Delete( OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks );


/** @brief Clears a struct for the global definition callbacks.
 *
 *  @param globalDefReaderCallbacks Handle to a struct previously allocated
 *                                  with @eref{OTF2_GlobalDefReaderCallbacks_New}.
 */
void
OTF2_GlobalDefReaderCallbacks_Clear( OTF2_GlobalDefReaderCallbacks* globalDefReaderCallbacks );


/** @brief Function pointer definition for the callback which is triggered by
 *         an unknown definition record.
 *
 *  @param userData User data as set by
 *                  @eref{OTF2_Reader_RegisterGlobalDefCallbacks} or
 *                  @eref{OTF2_GlobalDefReader_SetCallbacks}.
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( *OTF2_GlobalDefReaderCallback_Unknown )( void* userData );


/** @brief Registers the callback for an unknown definition.
 *
 *  @param globalDefReaderCallbacks Struct for all callbacks.
 *  @param unknownCallback          Function which should be called for all
 *                                  Unknown definitions.
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_GlobalDefReaderCallbacks_SetUnknownCallback(
    OTF2_GlobalDefReaderCallbacks*       globalDefReaderCallbacks,
    OTF2_GlobalDefReaderCallback_Unknown unknownCallback );

@otf2 for def in defs|global_defs:
@otf2 with arg_name = def.lname + 'Callback'

/** @brief Function pointer definition for the callback which is triggered by
 *         a @eref{@@def.name@@} definition record.
 *
 *  @@def.doxygendoc()@@
 *  @@def.doxygenargs(prepend=[('userData', 'User data as set by @eref{OTF2_Reader_RegisterGlobalDefCallbacks} or @eref{OTF2_GlobalDefReader_SetCallbacks}.')])@@
 *
 *  @since Version @@def.since@@
 *
@otf2 if def.deprecated:
 *  @deprecated In version @@def.deprecated@@
 *
@otf2 endif
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( *OTF2_GlobalDefReaderCallback_@@def.name@@ )( void* userData @@def.funcargs()@@ );


/** @brief Registers the callback for the @eref{@@def.name@@} definition.
 *  @@doxygenparams([
    ('globalDefReaderCallbacks', 'Struct for all callbacks.'),
    (arg_name,                   'Function which should be called for all @eref{%s} definitions.' % def.name)])@@
 *
 *  @since Version @@def.since@@
 *
@otf2 if def.deprecated:
 *  @deprecated In version @@def.deprecated@@
 *
@otf2 endif
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_GlobalDefReaderCallbacks_Set@@def.name@@Callback(
    OTF2_GlobalDefReaderCallbacks*            globalDefReaderCallbacks,
    OTF2_GlobalDefReaderCallback_@@def.name@@ @@arg_name@@ );

@otf2 endwith
@otf2 endfor

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* !OTF2_GLOBAL_DEF_READER_CALLBACKS_H */
