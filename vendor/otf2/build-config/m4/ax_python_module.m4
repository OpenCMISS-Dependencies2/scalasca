# ===========================================================================
#     http://www.gnu.org/software/autoconf-archive/ax_python_module.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PYTHON_MODULE(modname[, fatal, python])
#
# DESCRIPTION
#
#   Checks for Python module.
#
#   If fatal is non-empty then absence of a module will trigger an error.
#   The third parameter can either be "python" for Python 2 or "python3" for
#   Python 3; defaults to Python 3.
#
# LICENSE
#
#   Copyright (c) 2008 Andrew Collier
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 8

AU_ALIAS([AC_PYTHON_MODULE], [AX_PYTHON_MODULE])
AC_DEFUN([AX_PYTHON_MODULE],[
    m4_pushdef([_ax_python_module], AS_TR_CPP(HAVE_PYMOD_$1))dnl
    m4_pushdef([_ax_python_module_version], AS_TR_CPP(HAVE_PYMOD_VERSION_$1))dnl
    if test -z $PYTHON;
    then
        if test -z "$3";
        then
            PYTHON="python3"
        else
            PYTHON="$3"
        fi
    fi
    _ax_python_name=`basename $PYTHON`
    AC_MSG_CHECKING($_ax_python_name module: $1)

    _ax_python_import='$PYTHON -c '\''import $1'\'' >&AS_MESSAGE_LOG_FD'
    AS_IF([_AC_DO_VAR([_ax_python_import])], [
        _ax_python_module_version=$($PYTHON -c "import $1; print($1.__version__)" 2>/dev/null)
        AC_MSG_RESULT([yes${]_ax_python_module_version[:+, version $]_ax_python_module_version[}])
        _ax_python_module=yes
    ], [
        AC_MSG_RESULT([no])
        _ax_python_module=no
        m4_ifnblank($2, AC_MSG_FAILURE([failed to find required module ]$1))
    ])

    AS_UNSET([_ax_python_import])
    AS_UNSET([_ax_python_name])
    m4_popdef([_ax_python_module_version])
    m4_popdef([_ax_python_module])
])
