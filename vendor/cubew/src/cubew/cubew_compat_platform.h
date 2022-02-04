/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2015                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/**
 * \file cubew_compat_platform.h...
 * \brief Header with definitions and on other than linux platforms missing calls or with different declaration.
 *
 *
 *
 */


#ifndef CUBEW_COMPAT_PLATFORM_H
#define CUBEW_COMPAT_PLATFORM_H

#ifdef __MINGW32__ // for windows we compile only on MinGW32. Here are some calls different or missing

#include <unistd.h>
#include <sys/types.h>

// on windows there is no "mode" for directory creation
#define mkdir( path, mode ) mkdir( path )

// redefine fseeko with standart fseek
// #warning "On MinGW Platform we redefine fseeko using standard fseek".
#define fseeko fseek

typedef unsigned int gid_t;
typedef unsigned int uid_t;


gid_t
getgid( void );

uid_t
getuid( void );




#endif

#endif
