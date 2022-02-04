/* ../src/config-backend.h.  Generated from config-backend.h.in by configure.  */
/* ../src/config-backend.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Name of the sub-build. */
#define AFS_PACKAGE_BUILD "backend"

/* Symbol name of the sub-build in upper case. */
#define AFS_PACKAGE_BUILD_NAME BACKEND

/* Symbol name of the sub-build in lower case. */
#define AFS_PACKAGE_BUILD_name backend

/* The package name usable as a symbol in upper case. */
#define AFS_PACKAGE_NAME OTF2

/* Relative path to the top-level source directory. */
#define AFS_PACKAGE_SRCDIR "../"

/* The package name usable as a symbol in lower case. */
#define AFS_PACKAGE_name otf2

/* Try to use colorful output for tests. */
/* #undef CUTEST_USE_COLOR */

/* Define if you have POSIX threads libraries and header files. */
#define HAVE_BACKEND_PTHREAD 1

/* Support for pthread_barrier_t. */
#define HAVE_BACKEND_PTHREAD_BARRIER 1

/* Support for PTHREAD_MUTEX_ERRORCHECK. */
#define HAVE_BACKEND_PTHREAD_MUTEX_ERRORCHECK 1

/* Defined if SIONlib is available. */
#define HAVE_BACKEND_SIONLIB_SUPPORT 0

/* Defined to 1 if the clock_gettime() function is available. */
/* #undef HAVE_CLOCK_GETTIME */

/* Define to 1 if you have the declaration of `fseeko', and to 0 if you don't.
   */
#define HAVE_DECL_FSEEKO 0

/* Define to 1 if you have the declaration of `fseeko64', and to 0 if you
   don't. */
#define HAVE_DECL_FSEEKO64 1

/* Define to 1 if you have the declaration of `getcwd', and to 0 if you don't.
   */
#define HAVE_DECL_GETCWD 1

/* Define to 1 if you have the declaration of `gethostid', and to 0 if you
   don't. */
#define HAVE_DECL_GETHOSTID 0

/* Define to 1 if you have the declaration of `gethostname', and to 0 if you
   don't. */
#define HAVE_DECL_GETHOSTNAME 0

/* Define to 1 if you have the <dlfcn.h> header file. */
/* #undef HAVE_DLFCN_H */

/* Can link a fseeko function */
/* #undef HAVE_FSEEKO */

/* Can link a fseeko64 function */
#define HAVE_FSEEKO64 1

/* Can link a getcwd function */
#define HAVE_GETCWD 1

/* Can link a gethostid function */
/* #undef HAVE_GETHOSTID */

/* Can link a gethostname function */
/* #undef HAVE_GETHOSTNAME */

/* Define to 1 if the getpid() function is available. */
#define HAVE_GETPID 1

/* Define to 1 if the gettimeofday() function is available. */
#define HAVE_GETTIMEOFDAY 0

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <io.h> header file. */
#define HAVE_IO_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Defined to 1 if native MIC build exists */
/* #undef HAVE_MIC_SUPPORT */

/* Define to 1 to enable additional debug output and checks. */
/* #undef HAVE_OTF2_DEBUG */

/* Define to 1 to disable assertions (like NDEBUG). */
#define HAVE_OTF2_NO_ASSERT 0

/* Set if we are building for the AIX platform */
/* #undef HAVE_PLATFORM_AIX */

/* Set if we are building for the ALTIX platform */
/* #undef HAVE_PLATFORM_ALTIX */

/* Set if we are building for the ARM platform */
/* #undef HAVE_PLATFORM_ARM */

/* Set if we are building for the BG/L platform */
/* #undef HAVE_PLATFORM_BGL */

/* Set if we are building for the BG/P platform */
/* #undef HAVE_PLATFORM_BGP */

/* Set if we are building for the BG/Q platform */
/* #undef HAVE_PLATFORM_BGQ */

/* Set if we are building for the Cray platform */
/* #undef HAVE_PLATFORM_CRAY */

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
/* #undef HAVE_PLATFORM_LINUX */

/* Set if we are building for the Mac platform */
/* #undef HAVE_PLATFORM_MAC */

/* Set if we are building for the Intel MIC platform */
/* #undef HAVE_PLATFORM_MIC */

/* Set if we are building for the MinGW platform */
#define HAVE_PLATFORM_MINGW 1

/* Set if we are building for the NEC SX platform */
/* #undef HAVE_PLATFORM_NECSX */

/* Set if we are building for the Solaris platform */
/* #undef HAVE_PLATFORM_SOLARIS */

/* Can link against popen and pclose */
#define HAVE_POPEN 1

/* Define if you have POSIX threads libraries and header files. */
#define HAVE_PTHREAD 1

/* Support for pthread_barrier_t. */
#define HAVE_PTHREAD_BARRIER 1

/* Support for PTHREAD_MUTEX_ERRORCHECK. */
#define HAVE_PTHREAD_MUTEX_ERRORCHECK 1

/* Have PTHREAD_PRIO_INHERIT. */
/* #undef HAVE_PTHREAD_PRIO_INHERIT */

/* Defined if SIONlib is available. */
#define HAVE_SIONLIB_SUPPORT 0

/* Define to 1 if the stat() function is available. */
#define HAVE_STAT 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if the sysinfo() function is available. */
/* #undef HAVE_SYSINFO */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/sysinfo.h> header file. */
/* #undef HAVE_SYS_SYSINFO_H */

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 0

/* Interface version number */
#define LIBRARY_INTERFACE_VERSION "7:0:0"

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Portable mkdir that handles implementations which do not have the mode_t
   argument */
#define OTF2_MKDIR( path, mode ) mkdir( path )

/* Name of package */
#define PACKAGE "otf2"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "support@score-p.org"

/* The #include argument used to include this packages error codes header. */
#define PACKAGE_ERROR_CODES_HEADER <otf2/OTF2_ErrorCodes.h>

/* Define to the full name of this package. */
#define PACKAGE_NAME "OTF2"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "OTF2 2.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "otf2"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.0"

/* Directory where OTF2 executables to be run by other programs rather than by
   users are installed */
#define PKGLIBEXECDIR "/opt/otf2/libexec/otf2"

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Revision of common repository */
#define SCOREP_COMMON_REVISION "2076"

/* Revision of OTF2 */
#define SCOREP_COMPONENT_REVISION "4226"

/* Defined if we are working from svn. */
/* #undef SCOREP_IN_DEVELOPEMENT */

/* Defined if we are working from a make dist generated tarball. */
#define SCOREP_IN_PRODUCTION /**/

/* Defined if we are building shared libraries. See also SCOREP_STATIC_BUILD
   */
/* #undef SCOREP_SHARED_BUILD */

/* Defined if we are building static libraries. See also SCOREP_SHARED_BUILD
   */
#define SCOREP_STATIC_BUILD /**/

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "2.0"

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
