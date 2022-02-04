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


#ifndef OTF2_INTERNAL_EVENTS_H
#define OTF2_INTERNAL_EVENTS_H


/**
 *  @internal
 *
 *  @file
 *  @source     templates/otf2_events_int.tmpl.h
 *
 *  @brief      This file list all event records supported in OTF2.
 */


typedef uint8_t OTF2_EventType;


enum OTF2_EventType_enum
{
    /* First five entries are reserved for OTF2 internals */

    @otf2 for event in events:
    OTF2_EVENT_@@event.upper@@ = @@event.id@@,  /** Event record identifier for the @@event.name@@ event. */
    @otf2 endfor

    OTF2_EVENT_MAX_ID
};

@otf2 for event in events:

/** @brief @@event.name@@ event.
 */
typedef struct OTF2_@@event.name@@_struct
{
    /** Timestamp, which is exactly here in every event record */
    OTF2_TimeStamp time;
    @@event.structargs(attributes=event.record_attributes, case='lower')@@
} OTF2_@@event.name@@;

@otf2 endfor

typedef struct OTF2_GenericEvent_struct OTF2_GenericEvent;


struct OTF2_GenericEvent_struct
{
    OTF2_EventType type;
    union {
        OTF2_TimeStamp time;   /**< Timestamp, which is exactly here in every record */
        @otf2 for event in events:
        OTF2_@@event.name@@ @@event.lower@@;
        @otf2 endfor
    } record;
};


struct OTF2_EvtReaderCallbacks_struct
{
    OTF2_EvtReaderCallback_Unknown unknown;
    @otf2 for event in events:
    OTF2_EvtReaderCallback_@@event.name@@ @@event.lower@@;
    @otf2 endfor
};


struct OTF2_GlobalEvtReaderCallbacks_struct
{
    OTF2_GlobalEvtReaderCallback_Unknown unknown;
    @otf2 for event in events:
    OTF2_GlobalEvtReaderCallback_@@event.name@@ @@event.lower@@;
    @otf2 endfor
};

@otf2 for event in events if event.alternative_event:

bool otf2_event_@@event.lower@@_convert_to_@@event.alternative_event.lower@@( OTF2_Archive*                          archive,
                                                                              const OTF2_@@event.name@@*             @@event.lname@@Record,
                                                                              OTF2_AttributeList*                    attributeList,
                                                                              OTF2_@@event.alternative_event.name@@* @@event.alternative_event.lname@@Record );

@otf2 endfor

#endif /* OTF2_INTERNAL_EVENTS_H */
