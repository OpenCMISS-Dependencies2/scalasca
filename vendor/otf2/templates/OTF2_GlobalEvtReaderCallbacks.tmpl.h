/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
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


#ifndef OTF2_GLOBAL_EVT_READER_CALLBACKS_H
#define OTF2_GLOBAL_EVT_READER_CALLBACKS_H


/**
 *  @file
 *  @source     templates/OTF2_GlobalEvtReaderCallbacks.tmpl.h
 *
 *  @brief      This defines the callbacks for the global event reader.
 */


#include <stdint.h>


#include <otf2/OTF2_ErrorCodes.h>


#include <otf2/OTF2_GeneralDefinitions.h>
#include <otf2/OTF2_AttributeList.h>
#include <otf2/OTF2_Events.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief Opaque struct which holds all event record callbacks.
 */
typedef struct OTF2_GlobalEvtReaderCallbacks_struct OTF2_GlobalEvtReaderCallbacks;


/** @brief Allocates a new struct for the event callbacks.
 *
 *  @return A newly allocated struct of type @eref{OTF2_GlobalEvtReaderCallbacks}.
 */
OTF2_GlobalEvtReaderCallbacks*
OTF2_GlobalEvtReaderCallbacks_New( void );


/** @brief Deallocates a struct for the global event callbacks.
 *
 *  @param globalEvtReaderCallbacks Handle to a struct previously allocated
 *                                  with @eref{OTF2_GlobalEvtReaderCallbacks_New}.
 */
void
OTF2_GlobalEvtReaderCallbacks_Delete( OTF2_GlobalEvtReaderCallbacks* globalEvtReaderCallbacks );


/** @brief Clears a struct for the global event callbacks.
 *
 *  @param globalEvtReaderCallbacks Handle to a struct previously allocated
 *                                  with @eref{OTF2_GlobalEvtReaderCallbacks_New}.
 */
void
OTF2_GlobalEvtReaderCallbacks_Clear( OTF2_GlobalEvtReaderCallbacks* globalEvtReaderCallbacks );


/** @brief Callback for an unknown event record.
 *
 *  @param locationID    The location where this event happened.
 *  @param time          The time when this event happened.
 *  @param userData      User data as set by
 *                       @eref{OTF2_Reader_RegisterGlobalEvtCallbacks} or
 *                       @eref{OTF2_GlobalEvtReader_SetCallbacks}.
 *  @param attributeList Additional attributes for this event.
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( *OTF2_GlobalEvtReaderCallback_Unknown )( OTF2_LocationRef    locationID,
                                           OTF2_TimeStamp      time,
                                           void*               userData,
                                           OTF2_AttributeList* attributeList );


/** @brief Registers the callback for unknown events.
 *
 *  @param globalEvtReaderCallbacks Struct for all callbacks.
 *  @param unknownCallback          Function which should be called for all
 *                                  unknown events.
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_GlobalEvtReaderCallbacks_SetUnknownCallback(
    OTF2_GlobalEvtReaderCallbacks*       globalEvtReaderCallbacks,
    OTF2_GlobalEvtReaderCallback_Unknown unknownCallback );

@otf2 for event in events:
@otf2 with arg_name = event.lname + 'Callback'

/** @brief Callback for the @@event.name@@ event record.
 *
 *  @@event.doxygendoc()@@
 *  @@event.doxygenargs(prepend=[
    ('locationID',    'The location where this event happened.'),
    ('time',          'The time when this event happened.'),
    ('userData',      'User data as set by @eref{OTF2_Reader_RegisterGlobalEvtCallbacks} or @eref{OTF2_GlobalEvtReader_SetCallbacks}.'),
    ('attributeList', 'Additional attributes for this event.')])@@
 *
 *  @since Version @@event.since@@
 *
 *  @return @eref{OTF2_CALLBACK_SUCCESS} or @eref{OTF2_CALLBACK_INTERRUPT}.
 */
typedef OTF2_CallbackCode
( *OTF2_GlobalEvtReaderCallback_@@event.name@@ )( OTF2_LocationRef    locationID,
                                                  OTF2_TimeStamp      time,
                                                  void*               userData,
                                                  OTF2_AttributeList* attributeList@@event.funcargs()@@ );


/** @brief Registers the callback for the @@event.name@@ event.
 *  @@doxygenparams([
    ('globalEvtReaderCallbacks', 'Struct for all callbacks.'),
    (arg_name,                   'Function which should be called for all @eref{%s} definitions.' % event.name)])@@
 *
 *  @since Version @@event.since@@
 *
 *  @retbegin
 *    @retcode{OTF2_SUCCESS, if successful}
 *    @retcode{OTF2_ERROR_INVALID_ARGUMENT,
 *             for an invalid @p defReaderCallbacks argument}
 *  @retend
 */
OTF2_ErrorCode
OTF2_GlobalEvtReaderCallbacks_Set@@event.name@@Callback(
    OTF2_GlobalEvtReaderCallbacks*              globalEvtReaderCallbacks,
    OTF2_GlobalEvtReaderCallback_@@event.name@@ @@arg_name@@ );

@otf2 endwith
@otf2 endfor

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* !OTF2_GLOBAL_EVT_READER_CALLBACKS_H */
