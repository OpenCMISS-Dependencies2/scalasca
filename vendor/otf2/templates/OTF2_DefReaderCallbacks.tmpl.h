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


#ifndef OTF2_DEF_READER_CALLBACKS_H
#define OTF2_DEF_READER_CALLBACKS_H


/**
 *  @file
 *  @source     templates/OTF2_DefReaderCallbacks.tmpl.h
 *
 *  @brief      This defines the callbacks for the definition reader.
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

/** @brief Opaque struct which holds all definition record callbacks.
 */
typedef struct OTF2_DefReaderCallbacks_struct OTF2_DefReaderCallbacks;


/** @brief Allocates a new struct for the definition callbacks.
 *
 *  @return A newly allocated struct of type @eref{OTF2_DefReaderCallbacks}.
 */
OTF2_DefReaderCallbacks*
OTF2_DefReaderCallbacks_New( void );


/** @brief Deallocates a struct for the definition callbacks.
 *
 *  @param defReaderCallbacks Handle to a struct previously allocated
 *                            with @eref{OTF2_DefReaderCallbacks_New}.
 */
void
OTF2_DefReaderCallbacks_Delete( OTF2_DefReaderCallbacks* defReaderCallbacks );


/** @brief Clears a struct for the definition callbacks.
 *
 *  @param defReaderCallbacks Handle to a struct previously allocated
 *                            with @eref{OTF2_DefReaderCallbacks_New}.
 */
void
OTF2_DefReaderCallbacks_Clear( OTF2_DefReaderCallbacks* defReaderCallbacks );


/** @brief Function pointer definition for the callback which is
 *         triggered for an unknown definition.
 *
 *  @param userData User data as set by
 *                  @eref{OTF2_Reader_RegisterDefCallbacks} or
 *                  @eref{OTF2_DefReader_SetCallbacks}.
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( *OTF2_DefReaderCallback_Unknown )( void* userData );


/** @brief Registers the callback for an unknown definition.
 *
 *  @param defReaderCallbacks Struct for all callbacks.
 *  @param unknownCallback    Function which should be called for all
 *                            unknown definitions.
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_DefReaderCallbacks_SetUnknownCallback(
    OTF2_DefReaderCallbacks*       defReaderCallbacks,
    OTF2_DefReaderCallback_Unknown unknownCallback );

@otf2 for def in defs|local_defs:
@otf2 with arg_name = def.lname + 'Callback'

/** @brief Function pointer definition for the callback which is triggered by
 *         a @eref{@@def.name@@} definition record.
 *
 *  @@def.doxygendoc()@@
 *  @@def.doxygenargs(prepend=[('userData', 'User data as set by @eref{OTF2_Reader_RegisterDefCallbacks} or @eref{OTF2_DefReader_SetCallbacks}.')])@@
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
( *OTF2_DefReaderCallback_@@def.name@@ )( void* userData @@def.funcargs()@@ );


/** @brief Registers the callback for the @eref{@@def.name@@} definition.
 *  @@doxygenparams([
    ('defReaderCallbacks', 'Struct for all callbacks.'),
    (arg_name,             'Function which should be called for all @eref{%s} definitions.' % def.name)])@@
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
OTF2_DefReaderCallbacks_Set@@def.name@@Callback(
    OTF2_DefReaderCallbacks*            defReaderCallbacks,
    OTF2_DefReaderCallback_@@def.name@@ @@arg_name@@ );

@otf2 endwith
@otf2 endfor

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* !OTF2_DEF_READER_CALLBACKS_H */
