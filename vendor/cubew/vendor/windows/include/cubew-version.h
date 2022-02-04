/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#ifndef CUBEW_VERSION_H
#define CUBEW_VERSION_H


/*-------------------------------------------------------------------------*/
/**
 *  @file  CUBEW-version.h
 *  @brief Provides the CUBEW package version information.
 **/
/*-------------------------------------------------------------------------*/


/// @name CUBEW version information
/// @{

/// CUBEW revision number
#define CUBEW_REVISION "97a1f6d"

/// CUBEW revision number
#define CUBEW_REVISION_NUMBER 97a1f6d

/// CUBEW revision number
#define CUBEW_REVISION_STRING CUBEW_REVISION


/// Cube major version number
#define CUBEW_MAJOR_NUM   4

/// Cube minor version number
#define CUBEW_MINOR_NUM   6

/// Cube bugfix version number
#define CUBEW_BUGFIX_NUM  0

/// CUBEW major version number
#define CUBEW_MAJOR   "4"

/// CUBEW minor version number
#define CUBEW_MINOR   "6"

/// CUBEW bugfix version number
#define CUBEW_BUGFIX   "0"

/// CUBEW suffix
#define CUBEW_SUFFIX   ""

/// CUBEW suffix_internally
#define CUBEW_SUFFIX_INTERNAL   

/// CUBEW version number (<i>major</i>.<i>minor</i>)
#define CUBEW_SHORT_VERSION   CUBEW_MAJOR "." CUBEW_MINOR

/// CUBEW full version number (<i>major</i>.<i>minor</i>.<i>bugfix</i>)
#define CUBEW_FULL_VERSION   CUBEW_SHORT_VERSION "." CUBEW_BUGFIX

/// CUBEW version number
#define CUBEW_VERSION "4.6"

/// CUBEW package name "CUBEW"
#define CUBEW_SHORT_NAME  "CubeW"

/// CUBEW package name "CUBEW-4.x.1-XX"
#define CUBEW_FULL_NAME   CUBEW_SHORT_NAME "-" CUBEW_FULL_VERSION CUBEW_SUFFIX

/// usage: #if (CUBEW_VERSION_NUMBER >= CUBEW_VERSION_CHECK(4, 5, 0))'
/// Macro to build a single number out of major.minor.bugfix
#define CUBEW_VERSION_CHECK( major, minor, bugfix ) ( ( major << 16 ) | ( minor << 8 ) | ( bugfix ) )

/// usage: #if (CUBEW_VERSION_NUMBER >= CUBEW_VERSION_CHECK(4, 5, 0))'
/// Cube package version as a single number
#define CUBEW_VERSION_NUMBER CUBEW_VERSION_CHECK( CUBEW_MAJOR_NUM, CUBEW_MINOR_NUM, CUBEW_BUGFIX_NUM )

/// CUBEWW library interface information
#define LIBRARY_CURRENT "10"
#define LIBRARY_REVISION "0"
#define LIBRARY_AGE "3"

/// @}


#endif   // !CUBEW_VERSION_H
