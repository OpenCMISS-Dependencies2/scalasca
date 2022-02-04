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

/**
 *  @internal
 *
 *  @file
 */

#ifndef OTF2_INTERNAL_GLOBAL_DEF_READER_H
#define OTF2_INTERNAL_GLOBAL_DEF_READER_H


OTF2_GlobalDefReader*
otf2_global_def_reader_new( OTF2_Archive* archive );

OTF2_ErrorCode
otf2_global_def_reader_delete( OTF2_GlobalDefReader* reader );

OTF2_ErrorCode
otf2_global_def_reader_open_file( OTF2_GlobalDefReader* reader );


#endif /* OTF2_INTERNAL_GLOBAL_DEF_READER_H */
