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


/**
 *  @internal
 *
 *  @file
 *  @source     templates/otf2_attic_types.tmpl.h
 *
 *  @brief      Holds obsolete types.
 */


#ifndef OTF2_INTERNAL_ATTIC_TYPES_H
#define OTF2_INTERNAL_ATTIC_TYPES_H

@otf2 for enum in attic.enums:

/** @brief Wrapper for enum @eref{@@enum.type@@_enum}. */
typedef @@enum.type.c_orig@@ @@enum.type@@;

/**
 * @brief @@enum.doxygendoc(prefix=' * ')@@
 *
 * @since Version @@enum.since@@, removed in version @@enum.attic@@.
 */
enum @@enum.type@@_enum
{
    @otf2 for entry in enum.entries:
    /** @brief @@entry.doxygendoc(prefix='     *  ')@@
     */
    @@entry.name@@ = @@entry.value@@@@enum.need_comma(entry)@@
    @otf2 endfor
    @otf2 if enum is enum_with_terminator:

    /** @brief @@enum.terminator.doxygendoc(prefix='     *  ')@@
     */
    @@enum.terminator.name@@ = @@enum.terminator.value@@
    @otf2 endif
};

@otf2 endfor

#endif /* OTF2_INTERNAL_ATTIC_TYPES_H */
