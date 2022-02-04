##*************************************************************************##
##  CUBE        http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2016                                                ##
##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
##                                                                         ##
##  Copyright (c) 2009-2015                                                ##
##  German Research School for Simulation Sciences GmbH,                   ##
##  Laboratory for Parallel Programming                                    ##
##                                                                         ##
##  This software may be modified and distributed under the terms of       ##
##  a BSD-style license.  See the COPYING file in the package base         ##
##  directory for details.                                                 ##
##*************************************************************************##



## file       ac_cube_posix_functions.m4

AC_DEFUN([AC_CUBE_POSIX_FUNCTIONS], [

    ##
    ## check function declarations
    ##

    AC_LANG_PUSH(C)

    ## check whether functions are declared and set HAVE_DECL_* appropriately
    AC_CHECK_DECLS([gethostid, gethostname, fseeko, fseeko64, getcwd], [], [], [[
      #include <unistd.h>
      #include <stdio.h>
    ]])

    ##
    ## try to link
    ##

    has_gethostid_func="yes"
    AC_MSG_CHECKING([for gethostid])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include <stddef.h>

            #if !HAVE_DECL_GETHOSTID
            long gethostid(void);
            #endif

            int main()
            {
                long host_id = gethostid();
                return 0;
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_GETHOSTID, 1, [Can link a gethostid function])],
        [AC_MSG_RESULT(no)
         has_gethostid_func="no"]
    ) # AC_LINK_IF_ELSE

    has_gethostname_func="yes"
    AC_MSG_CHECKING([for gethostname])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include <stddef.h>

            #if !HAVE_DECL_GETHOSTNAME
            int gethostname(char *name, size_t namelen);
            #endif

            int main()
            {
                char* name;
                return gethostname(name, 256);
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_GETHOSTNAME, 1, [Can link a gethostname function])],
        [AC_MSG_RESULT(no)
         has_gethostname_func="no"]
    ) # AC_LINK_IF_ELSE

    has_fseeko_func="yes"
    AC_MSG_CHECKING([for fseeko])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include <stdio.h>
            #include <fcntl.h>

            #if !HAVE_DECL_FSEEKO
            int fseeko(FILE *stream, off_t offset, int whence);
            #endif

            int main()
            {
                FILE* stream;
                return fseeko(stream, 256, 0);
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_FSEEKO, 1, [Can link a fseeko function])],
        [AC_MSG_RESULT(no)
         has_fseeko_func="no"]
    ) # AC_LINK_IF_ELSE

    has_fseeko64_func="yes"
    AC_MSG_CHECKING([for fseeko64])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include <stdio.h>
            #include <unistd.h>
            #include <fcntl.h>

            #if !HAVE_DECL_FSEEKO64
            int fseeko64(FILE *stream, off64_t offset, int whence);
            #endif

            int main()
            {
                FILE* stream;
                return fseeko64(stream, 256lu, 0);
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_FSEEKO64, 1, [Can link a fseeko64 function])],
        [AC_MSG_RESULT(no)
         has_fseeko64_func="no"]
    ) # AC_LINK_IF_ELSE

    has_getcwd_func="yes"
    AC_MSG_CHECKING([for getcwd])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include <stdio.h>
            #include <unistd.h>
            #include <stdlib.h>

            #if !HAVE_DECL_GETCWD
            char* getcwd(char* buf, size_t size);
            #endif

            int main()
            {
                char* cwd = getcwd(NULL, 0);
                return (cwd == NULL ? EXIT_FAILURE : EXIT_SUCCESS);
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_GETCWD, 1, [Can link a getcwd function])],
        [AC_MSG_RESULT(no)
         has_getcwd_func="no"]
    ) # AC_LINK_IF_ELSE

    AC_LANG_POP(C)

    ##
    ## result
    ##

    AM_CONDITIONAL([HAVE_GETHOSTID], [test "x${has_gethostid_func}" = "xyes"])
    AM_CONDITIONAL([HAVE_GETHOSTNAME], [test "x${has_gethostname_func}" = "xyes"])
    AM_CONDITIONAL([HAVE_FSEEKO], [test "x${has_fseeko_func}" = "xyes"])
    AM_CONDITIONAL([HAVE_FSEEKO64], [test "x${has_fseeko64_func}" = "xyes"])
    AM_CONDITIONAL([HAVE_GETCWD], [test "x${has_getcwd_func}" = "xyes"])
])
