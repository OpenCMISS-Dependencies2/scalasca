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


# scorep--backend
## Options passed to aclocal, autoreconf, autopoint and gettextize.
## Specifies local macro directory.
## ACLOCAL_AMFLAGS contains options to pass to aclocal when aclocal.m4 is to be
## rebuilt by make. This line is also used by autoreconf to run aclocal with
## suitable options, or by autopoint and gettextize to locate the place where
## Gettext's macros should be installed. So even if you do not really care about
## the rebuild rules, you should define ACLOCAL_AMFLAGS.
## Can't be moved to common.am
ACLOCAL_AMFLAGS = -I ../build-config/m4 -I ../build-config/common/m4

## The SUBDIRS variable holds a list of subdirectories automake (recursivly)
## must build.
#SUBDIRS =

## Additional libtool linking flags
LINKMODE_FLAGS = @AFS_LINKMODE_LDFLAGS@


include ../build-includes/common.am


AM_CPPFLAGS += -DBACKEND_BUILD_NOMPI $(BACKEND_LIBZ_HEADERS)
AM_LDFLAGS += $(BACKEND_LIBZ_LD)

bindir = @bindir@@backend_suffix@
libdir = @libdir@@backend_suffix@

if CROSS_BUILD

AM_CPPFLAGS += -DCROSS_BUILD $(BACKEND_LIBZ_HEADERS)
AM_LDFLAGS += $(BACKEND_LIBZ_LD)

include ../build-includes/backend-only.am
include ../build-includes/front-and-backend.am


else

AM_CPPFLAGS += -DNOCROSS_BUILD $(FRONTEND_LIBZ_HEADERS) $(BACKEND_LIBZ_HEADERS)
AM_LDFLAGS += $(FRONTEND_LIBZ_LD) $(BACKEND_LIBZ_LD)
include ../build-includes/backend-only.am
include ../build-includes/front-and-backend.am
include ../build-includes/frontend-only.am


TESTS+= $(CUBE_FRONTEND_SELF_TESTS)
#if BACKEND_TEST_RUNS
#TESTS+=$(CUBE_BACKEND_SELF_TESTS)
#$(CUBE_C_WRITER_TESTS) $(CUBE_PARALLEL_WRITER_TESTS)
#$(CUBE_CPPLIB_TESTS) $(CUBE_TOOLS_TESTS) $(CUBE_GUI_TESTS)


#endif

endif


if PLATFORM_MIC
AM_TARGET=/mic
else
AM_TARGET=
endif



#CONFIG_HEADER_BACKEND = ../src/scorep_config_tool_backend.h
#CONFIG_HEADER_FRONTEND = ../src/scorep_config_tool_frontend.h

DISTCLEANFILES += $(BUILT_SOURCES)  \
		$(builddir)/config.summary \
                $(builddir)/@PACKAGE@.summary \
                *.cubex \
                *.c.exe 

#if !BACKEND_TEST_RUNS
#BUILT_SOURCES += cube_tests
#
##cube_tests:
#	rm -f cube_tests
#	echo "# some cube tests" > cube_tests
#	for i in "$(CUBE_SELF_TESTS)"; do echo "$$i" >> cube_tests; done
#endif


## Convenience variable, used in common.am
BUILD_DIR=build-backend




#EXTRA_LOCAL_CLEAN_TARGETS = clean-local-cube-config-tool

clean-local:
	rm -f cube_tests lex.yy.c scanner.h yacc.c yacc.h

.PHONY: $(PHONY_TARGETS) $(EXTRA_LOCAL_CLEAN_TARGETS)

distclean-local:




if CROSS_BUILD
install-data-local: install-backend-examples
else
install-exec-local:
install-data-local: install-backend-examples install-frontend-examples
endif
	@echo "Install example building controlling Makefile..."
	@$(INSTALL_DATA) $(abs_builddir)/../examples/Makefile $(DESTDIR)$(datadir)/doc/@PACKAGE@/example
	@echo "-------------------Copy examples execution scripts ---------------"
# 	$(INSTALL_DATA) exec.@BATCHSYS@  $(DESTDIR)$(datadir)/@PACKAGE@/doc/example/backend



create-backend-examples-dir:
	@$(MKDIR_P) $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/backend


install-backend-examples: create-backend-examples-dir $(INST_BACKEND_EXAMPLES)
	@echo "== Install backend examples"; \
	for example in $(INST_BACKEND_EXAMPLES); do \
	echo  "   Install backend_example $$example"; \
	echo " $(INSTALL_DATA) $$example  $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/backend"; \
	$(INSTALL_DATA) $$example  $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/backend || exit $$?; \
	done
	@echo "   Install a Makefile.backend for building examples using installed cube"
	@$(INSTALL_DATA) $(abs_builddir)/../examples/Makefile.backend $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/backend


if !CROSS_BUILD
create-frontend-examples-dir:
	$(MKDIR_P) $(DESTDIR)$(datadir)/doc/@PACKAGE@/example


install-frontend-examples: create-frontend-examples-dir $(INST_FRONTEND_EXAMPLES) $(INST_FRONTEND_PLUGIN_EXAMPLES)
	@echo "== Install frontend examples"; \
	for example in $(INST_FRONTEND_EXAMPLES) dummy; do \
            if test $$example != dummy; then \
                echo  "   Install example $$example"; \
                echo " $(INSTALL_DATA) $$example $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/"; \
                $(INSTALL_DATA) $$example $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/ || exit $$?; \
            fi; \
	done
	@echo "   Install a Makefile for building frontend examples using installed cube"
	@$(INSTALL_DATA) $(abs_builddir)/../examples/Makefile.frontend $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/

endif



uninstall-local:
	rm -rf $(DESTDIR)$(datadir)/@PACKAGE@




if !CROSS_BUILD
installcheck-local:  installcheck-backend installcheck-frontend
else
installcheck-local:  installcheck-backend 
endif

installcheck-backend:
	$(MAKE) -f $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/backend/Makefile.backend && $(MAKE) -f $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/backend/Makefile.backend run

if !CROSS_BUILD
installcheck-frontend: 
	$(MAKE) -f $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/Makefile.frontend && $(MAKE) -f $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/Makefile.frontend run
endif



