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


#ifndef OTF2_INTERNAL_GLOBAL_DEFINITIONS_H
#define OTF2_INTERNAL_GLOBAL_DEFINITIONS_H


/**
 *  @internal
 *
 *  @file
 *  @source     templates/otf2_global_definitions.tmpl.h
 */


/** @brief Wrapper for the OTF2_GlobalDefinitionType_enum
 */
typedef uint8_t OTF2_GlobalDefinitionType;


/** @brief An enum which holds all values that are possible global definition types.
 */
enum OTF2_GlobalDefinitionType_enum
{
    /* First five entries are reserved for OTF2 internals */

    /* OTF2 Records */
    @otf2 for def in defs|global_defs:
    OTF2_GLOBAL_DEF_@@def.upper@@ = @@def.id@@, /**< */
    @otf2 endfor

    OTF2_GLOBAL_DEF_MAX_ID
};

@otf2 for def in defs|global_defs:

/** @brief @@def.name@@ definition.
 */
typedef struct OTF2_GlobalDef@@def.name@@_struct
{
    @@def.structargs(attributes=def.record_attributes, case='lower')@@
} OTF2_GlobalDef@@def.name@@;

@otf2 endfor

struct OTF2_GlobalDefReaderCallbacks_struct
{
    OTF2_GlobalDefReaderCallback_Unknown unknown;
    @otf2 for def in defs|global_defs:
    OTF2_GlobalDefReaderCallback_@@def.name@@ @@def.lower@@;
    @otf2 endfor
};


#endif /* OTF2_INTERNAL_GLOBAL_DEFINITIONS_H */
