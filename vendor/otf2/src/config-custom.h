/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef CONFIG_CUSTOM_H
#define CONFIG_CUSTOM_H


/** @internal
 *  @file
 *
 *  @brief      This file gets included by config.h (resp. config-frontend.h and
 *              config-backend.h) and contains supplementary macros to be used
 *              with the macros in config.h.
 */


#define UTILS_DEBUG_MODULES \
    UTILS_DEFINE_DEBUG_MODULE( ARCHIVE,          0 ), \
    UTILS_DEFINE_DEBUG_MODULE( READER,           1 ), \
    UTILS_DEFINE_DEBUG_MODULE( ANCHOR_FILE,      2 ), \
    UTILS_DEFINE_DEBUG_MODULE( COLLECTIVES,      3 ), \
    UTILS_DEFINE_DEBUG_MODULE( POSIX,            4 ), \
    UTILS_DEFINE_DEBUG_MODULE( SION,             5 ), \
    UTILS_DEFINE_DEBUG_MODULE( SION_RANK_MAP,    6 ), \
    UTILS_DEFINE_DEBUG_MODULE( SION_COLLECTIVES, 7 ), \
    UTILS_DEFINE_DEBUG_MODULE( LOCKS,            8 )


#endif /* CONFIG_CUSTOM_H */
