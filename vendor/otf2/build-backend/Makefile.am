## -*- mode: makefile -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2013,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2013,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2014,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2013,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2013, 2015, 2018,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2013,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2013,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file       build-backend/Makefile.am


## Options passed to aclocal to specify local macro directories. Although
## ACLOCAL_AMFLAGS should be replaced by AC_CONFIG_MACRO_DIRS, aclocal-1.13.4
## doesn't honor the paths provided to AC_CONFIG_MACRO_DIRS. See
## https://debbugs.gnu.org/cgi/bugreport.cgi?bug=21352
ACLOCAL_AMFLAGS = -I ../build-config/m4 -I ../build-config/common/m4

## Additional libtool linking flags
LINKMODE_FLAGS = @AFS_LINKMODE_LDFLAGS@

include ../build-includes/common.am

AM_CPPFLAGS += -DBACKEND_BUILD

bindir = @bindir@@backend_suffix@
libdir = @libdir@@backend_suffix@

# path where the frontend tools will be installed
tools_bindir = @bindir@

if CROSS_BUILD

AM_CPPFLAGS += -DCROSS_BUILD
include ../build-includes/backend-only.am
include ../build-includes/front-and-backend.am

# src/config-backend-for-frontend.h dependencies
$(srcdir)/../src/config-backend-for-frontend.h.in: $(srcdir)/../src/config-backend.h.in
	$(AM_V_GEN)$(srcdir)/../build-config/common/generate-config-backend-for-frontend.sh $< >$@

otf2_config_flags = --target backend

else

AM_CPPFLAGS += -DNOCROSS_BUILD
include ../build-includes/backend-only.am
include ../build-includes/front-and-backend.am
include ../build-includes/frontend-only.am

if BACKEND_TEST_RUNS
TESTS += $(SCOREP_TESTS)
XFAIL_TESTS += $(SCOREP_XFAIL_TESTS)
endif

otf2_config_flags =

endif !CROSS_BUILD

if !BACKEND_TEST_RUNS
BUILT_SOURCES += scorep_tests

CLEANFILES += scorep_tests

scorep_tests:
	$(AM_V_GEN)( \
	    echo "# Available tests:"; \
	    for i in $(SCOREP_TESTS); do echo "./$$i"; done; \
	) >$@
endif

## To be used by LIB_DIR_UTIL
BUILD_DIR=build-backend

install-data-local:

uninstall-local:
