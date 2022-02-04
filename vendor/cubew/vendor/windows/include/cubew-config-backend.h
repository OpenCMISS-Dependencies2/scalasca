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

/* ../src/cubew-config-backend.h.  Generated from cubew-config-backend.h.in by configure.  */
/* ../src/cubew-config-backend.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Name of the sub-build. */
#define AFS_PACKAGE_BUILD "backend"

/* Symbol name of the sub-build in upper case. */
#define AFS_PACKAGE_BUILD_NAME BACKEND

/* Symbol name of the sub-build in lower case. */
#define AFS_PACKAGE_BUILD_name backend

/* The package name usable as a symbol in upper case. */
#define AFS_PACKAGE_NAME CUBEW

/* Relative path to the top-level source directory. */
#define AFS_PACKAGE_SRCDIR "../../cubew/build-backend/../"

/* The package name usable as a symbol in lower case. */
#define AFS_PACKAGE_name cubew

/* Compression is enabled */
#define COMPRESSION 1

/* Enable usage of external memory allocator */
/* #undef HAVE_ADVANCED_MEMORY */

/* Define to 1 if you have the declaration of `fseeko', and to 0 if you don't.
   */
#define HAVE_DECL_FSEEKO 1

/* Define to 1 if you have the declaration of `fseeko64', and to 0 if you
   don't. */
#define HAVE_DECL_FSEEKO64 0

/* Define to 1 if you have the declaration of `getcwd', and to 0 if you don't.
   */
#define HAVE_DECL_GETCWD 1

/* Define to 1 if you have the declaration of `gethostid', and to 0 if you
   don't. */
#define HAVE_DECL_GETHOSTID 1

/* Define to 1 if you have the declaration of `gethostname', and to 0 if you
   don't. */
#define HAVE_DECL_GETHOSTNAME 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Can link a fseeko function */
#define HAVE_FSEEKO 1

/* Can link a fseeko64 function */
/* #undef HAVE_FSEEKO64 */

/* Can link a getcwd function */
#define HAVE_GETCWD 1

/* Can link a gethostid function */
#define HAVE_GETHOSTID 1

/* Can link a gethostname function */
#define HAVE_GETHOSTNAME 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Enable internal (detailed) tracing of memory usage */
/* #undef HAVE_MEMORY_TRACING */

/* Enable internal tracking of memory usage */
/* #undef HAVE_MEMORY_TRACKING */

/* Defined to 1 if native MIC build exists */
/* #undef HAVE_MIC_SUPPORT */

/* Set if we are building for the AIX platform */
/* #undef HAVE_PLATFORM_AIX */

/* Set if we are building for the ALTIX platform */
/* #undef HAVE_PLATFORM_ALTIX */

/* Set if we are building for the BG/L platform */
/* #undef HAVE_PLATFORM_BGL */

/* Set if we are building for the BG/P platform */
/* #undef HAVE_PLATFORM_BGP */

/* Set if we are building for the BG/Q platform */
/* #undef HAVE_PLATFORM_BGQ */

/* Set if we are building for the Cray platform */
/* #undef HAVE_PLATFORM_CRAY */

/* Set if we are building for an unknown Cray */
/* #undef HAVE_PLATFORM_CRAYUNKNOWN */

/* Set if we are building for the Cray XC platform */
/* #undef HAVE_PLATFORM_CRAYXC */

/* Set if we are building for the Cray XE platform */
/* #undef HAVE_PLATFORM_CRAYXE */

/* Set if we are building for the Cray XK platform */
/* #undef HAVE_PLATFORM_CRAYXK */

/* Set if we are building for the Cray XT platform */
/* #undef HAVE_PLATFORM_CRAYXT */

/* Set if we are building for the FX10 platform */
/* #undef HAVE_PLATFORM_FX10 */

/* Set if we are building for the FX100 platform */
/* #undef HAVE_PLATFORM_FX100 */

/* Set if we are building for the K platform */
/* #undef HAVE_PLATFORM_K */

/* Set if we are building for the Linux platform */
// #define HAVE_PLATFORM_LINUX 1

/* Set if we are building for the macOS platform */
/* #undef HAVE_PLATFORM_MAC */

/* Set if we are building for the Intel MIC platform */
/* #undef HAVE_PLATFORM_MIC */

/* Set if we are building for the MinGW platform */
#undef HAVE_PLATFORM_MINGW

/* Set if we are building for the NEC SX platform */
/* #undef HAVE_PLATFORM_NECSX */

/* Set if we are building for the Solaris platform */
/* #undef HAVE_PLATFORM_SOLARIS */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Interface version number */
#define LIBRARY_INTERFACE_VERSION "10:0:3"

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Name of package */
#define PACKAGE "cubew"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "scalasca@fz-juelich.de"

/* The #include argument used to include this packages error codes header. */
#define PACKAGE_ERROR_CODES_HEADER <CubeWErrorCodes.h>

/* Define to the full name of this package. */
#define PACKAGE_NAME "CubeW"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "CubeW 4.6"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "cubew"

/* Define to the home page for this package. */
#define PACKAGE_URL "http://www.scalasca.org"

/* Define to the version of this package. */
#define PACKAGE_VERSION "4.6"

/* Directory where CubeW data is stored */
#define PKGDATADIR ""

/* Directory where CubeW executables to be run by other programs rather than
   by users are installed */
#define PKGLIBEXECDIR ""

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "4.6"

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Define to 1 if needed to make fixed-width integer constant macros available
   in C++ */
/* #undef __STDC_CONSTANT_MACROS */

/* Define to 1 if needed to make fixed-width integer format macros available
   in C++ */
/* #undef __STDC_FORMAT_MACROS */

/* Define to 1 if needed to make fixed-width integer limit macros available in
   C++ */
/* #undef __STDC_LIMIT_MACROS */
