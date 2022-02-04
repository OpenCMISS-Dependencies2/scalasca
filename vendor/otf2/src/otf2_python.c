/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
 * Technische Universitaet Dresden, Germany
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
 *  @brief      Hidden functions for python bindings.
 */


#include <config.h>

#include <stdlib.h>

void
otf2_python_free( void* mem )
{
    free( mem );
}
