##*************************************************************************##
##  CUBE        http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2018                                                ##
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



# scorep--frontend

## Options passed to aclocal, autoreconf, autopoint and gettextize.
## Specifies local macro directory.
## ACLOCAL_AMFLAGS contains options to pass  aclocal when aclocal.m4 is to be
## rebuilt by make. This line is also used by autoreconf to run aclocal with
## suitable options, or by autopoint and gettextize to locate the place where
## Gettext's macros should be installed. So even if you do not really care about
## the rebuild rules, you should define ACLOCAL_AMFLAGS.
## Can't be moved to common.am

ACLOCAL_AMFLAGS = -I ../build-config/m4 -I ../build-config/common/m4

include ../build-includes/common.am
#include ../build-includes/mpi.targets.am

AM_CPPFLAGS += -DFRONTEND_BUILD -DCROSS_BUILD $(FRONTEND_LIBZ_HEADERS)
AM_LDFLAGS += $(FRONTEND_LIBZ_LD)


if PLATFORM_MIC
AM_TARGET=/mic
else
AM_TARGET=
endif


include ../build-includes/frontend-only.am
include ../build-includes/front-and-backend.am



TESTS+= $(CUBE_FRONTEND_SELF_TESTS)

#$(CUBE_CPPLIB_TESTS) $(CUBE_TOOLS_TEST) $(CUBE_GUI_TESTS)


## Convenience variable, used in common.am
BUILD_DIR=build-frontend



DISTCLEANFILES += $(BUILT_SOURCES)\
		$(builddir)/config.summary \
                $(builddir)/@PACKAGE@.summary

.PHONY: $(PHONY_TARGETS)


## Here we write only backend values.
create-config-tool-headers:
	(cd ../build-backend && $(MAKE) create-config-tool-headers)


install-data-local: create-frontend-examples-dir install-frontend-examples


create-frontend-examples-dir:

install-frontend-examples: $(INST_FRONTEND_EXAMPLES) $(INST_FRONTEND_PLUGIN_EXAMPLES)
	@echo "== Install frontend examples"; \
	for example in $(INST_FRONTEND_EXAMPLES) dummy; do \
            if test $$example != dummy; then \
                echo  "   Install example $$example"; \
                $(INSTALL_DATA) $$example $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/ || exit $$?; \
           fi; \
	done
	@echo "   Install a Makefile for building frontend examples using installed cube"
	@$(INSTALL_DATA) $(abs_builddir)/../examples/Makefile.frontend $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/


installcheck-local:  installcheck-frontend

installcheck-frontend: 
	$(MAKE) -f $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/Makefile.frontend && $(MAKE) -f $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/Makefile.frontend run



