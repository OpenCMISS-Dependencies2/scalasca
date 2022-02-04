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
 *
 *  @brief      Provides conversion functions for new and obsoleted record
 *              attributes.
 */


#ifndef OTF2_INTERNAL_ATTIC_H
#define OTF2_INTERNAL_ATTIC_H


OTF2_RegionType
otf2_attic_def_region_provide_region_type( OTF2_RegionRole regionRole,
                                           OTF2_Paradigm   paradigm,
                                           OTF2_RegionFlag regionFlags );

void
otf2_attic_def_region_convert_region_type( OTF2_RegionType  regionType,
                                           OTF2_RegionRole* regionRole,
                                           OTF2_Paradigm*   paradigm,
                                           OTF2_RegionFlag* regionFlags );


OTF2_GroupTypePre12
otf2_attic_def_group_provide_group_type_pre_1_2( OTF2_GroupType groupType,
                                                 OTF2_Paradigm  paradigm,
                                                 OTF2_GroupFlag groupFlags );

void
otf2_attic_def_group_convert_group_type_pre_1_2( OTF2_GroupTypePre12 groupTypePre12,
                                                 OTF2_GroupType*     groupType,
                                                 OTF2_Paradigm*      paradigm,
                                                 OTF2_GroupFlag*     groupFlags );


#endif /* OTF2_INTERNAL_ATTIC_H */
