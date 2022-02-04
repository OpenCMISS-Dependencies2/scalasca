## -*- mode: makefile -*-
##*************************************************************************##
##  SCALASCA    http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2021                                                ##
##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
##                                                                         ##
##  Copyright (c) 2009-2013                                                ##
##  German Research School for Simulation Sciences GmbH,                   ##
##  Laboratory for Parallel Programming                                    ##
##                                                                         ##
##  This software may be modified and distributed under the terms of       ##
##  a BSD-style license.  See the COPYING file in the package base         ##
##  directory for details.                                                 ##
##*************************************************************************##


## Subdirectories to be built recursively
SUBDIRS = $(subdirs) $(ax_config_subdirs)

## Options passed to aclocal to specify local macro directories. Although
## ACLOCAL_AMFLAGS should be replaced by AC_CONFIG_MACRO_DIRS, aclocal-1.13.4
## doesn't honor the paths provided to AC_CONFIG_MACRO_DIRS. See
## https://debbugs.gnu.org/cgi/bugreport.cgi?bug=21352
ACLOCAL_AMFLAGS = -I build-config/m4 -I build-config/common/m4

## Include Makefile.am snippets
include build-includes/common.am

## Convenience variables
DISTDIR = $(top_distdir)$(ac_scorep_doxygen_distdir)

## Additional custom installation directories
manualdir = $(docdir)/manual

## Initialize some variables to be able to use '+=' in Makefile.am snippets.
## This includes the `TESTS` variable, since non-color automake only provides
## $(am__tty_colors) if tests are in use.
BUILT_SOURCES =
CLEANFILES    =
EXTRA_DIST    =
TESTS         =
manual_DATA   =
PHONY_TARGETS =

## To install configuration summary file(s)
pkgdata_DATA = \
    @PACKAGE@.summary

if PLATFORM_MIC

# On MIC, install config.summary also in $pkgdatadir/mic
micpkgdatadir   = $(pkgdatadir)/mic
micpkgdata_DATA = \
    @PACKAGE@.summary

else !PLATFORM_MIC

TARGETS = mic

endif !PLATFORM_MIC

## To install extra documentation files
dist_doc_DATA = \
    README \
    OPEN_ISSUES

## To clean up configuration summary files
DISTCLEANFILES = \
    config.summary \
    @PACKAGE@.summary


##--- Hooks -----------------------------------------------------------------

dist-hook: \
    prohibit-make-dist-with-disabled-components \
    copy-revision-file-to-dist \
    copy-generator-parser-files-to-dist \
    copy-user-documentation-to-dist


install-data-local: \
    install-pearl-example


install-data-hook: \
    install-user-documentation
	$(AM_V_at)list='$(TARGETS)'; test -n "$$list" || exit 0; \
	for target in $$list; do \
	    if test -f $(DESTDIR)$(pkgdatadir)/$$target/@PACKAGE@.summary; then \
	        { \
	            echo; \
	            echo; \
	            echo "=====[ Target: $$target ]===================="; \
	            echo; \
	            cat "$(DESTDIR)$(pkgdatadir)/$$target/@PACKAGE@.summary"; \
	        } >>"$(DESTDIR)$(pkgdatadir)/@PACKAGE@.summary"; \
	    fi \
	done


install-exec-hook: \
    install-mic-wrapper

uninstall-hook: \
    uninstall-user-documentation \
    uninstall-mic-wrapper


clean-local: \
    clean-user-documentation \
    clean-developer-documentation


PHONY_TARGETS += \
    prohibit-make-dist-with-disabled-components
prohibit-make-dist-with-disabled-components:
	@$(am__tty_colors); \
        if test "x$(PROHIBIT_MAKE_DIST)" != "x"; then \
	    echo "$${red}error: Cannot create distribution because of disabled components or use of$$std"; \
	    echo "       $${red}external packages.  Rerun configure with these flags to ensure a$$std"; \
	    echo "       $${red}self-contained distribution:$$std"; \
	    echo; \
	    echo "           $${blu}$(PROHIBIT_MAKE_DIST)$$std"; \
	    echo; \
	    exit 1; \
	fi


##--- Extra distribution files ----------------------------------------------

include build-config/platforms/platforms.am
include build-config/common/platforms.am

EXTRA_DIST += \
    src/config.h \
    share/scalasca.errors \
    build-config/tap-driver.sh \
    build-config/common/generate-library-version.sh \
    build-config/common/generate-package-version.sh \
    build-config/common/test-runner-mpi.sh \
    build-config/common/test-runner-plain.sh

if !CROSS_BUILD

EXTRA_DIST += \
    build-frontend/configure.ac \
    build-frontend/aclocal.m4 \
    build-frontend/Makefile.am \
    build-frontend/Makefile.in \
    build-frontend/configure \
    src/config-frontend.h.in

endif !CROSS_BUILD


##--- REVISION file related targets -----------------------------------------

PHONY_TARGETS += \
    copy-revision-file-to-dist \
    update-revision-file

copy-revision-file-to-dist: update-revision-file
	$(AM_V_at)$(MKDIR_P) "$(DISTDIR)/build-config"
	$(AM_V_at)cp -fp "$(srcdir)/build-config/REVISION" "$(DISTDIR)/build-config/"
	$(AM_V_at)case `cat "$(DISTDIR)/build-config/REVISION"` in *-dirty) \
	    echo >&2 "WARNING: distribution does not match a single, unmodified revision.";; \
	esac

if GIT_CONTROLLED

update-revision-file:
	@$(am__tty_colors); \
	if $(AM_V_P); then echo "cd $(srcdir) && git describe --long --always --dirty | $(SED) 's/.*-g//' >'build-config/REVISION'"; \
	              else echo "  GEN      $${mgn}$(srcdir)/build-config/REVISION$$std"; fi; \
	$(am__cd) $(srcdir) && git describe --long --always --dirty | $(SED) 's/.*-g//' >"build-config/REVISION"

else !GIT_CONTROLLED

update-revision-file:

endif !GIT_CONTROLLED


##--- Targets related to error code headers ---------------------------------

DISTCLEANFILES += \
    src/ScalascaErrorCodes.h \
    src/scalasca_error_decls.gen.h

BUILT_SOURCES += \
    src/ScalascaErrorCodes.h \
    src/scalasca_error_decls.gen.h

src/ScalascaErrorCodes.h: \
    share/scalasca.errors
src/scalasca_error_decls.gen.h: \
    share/scalasca.errors


##--- Uncrustify-related targets --------------------------------------------

if GIT_CONTROLLED
if HAVE_UNCRUSTIFY

beautify-local:
	@$(am__cd) $(srcdir) && ./common/beautifier/beautify

endif HAVE_UNCRUSTIFY
endif GIT_CONTROLLED


##--- Include Makefile.am snippets ------------------------------------------

include doc/Makefile.top.inc.am
include doc/doxygen-common/Makefile.top.inc.am
include doc/doxygen-dev/pearl/Makefile.top.inc.am
include doc/doxygen-dev/scout/Makefile.top.inc.am
include doc/doxygen-dev/silas/Makefile.top.inc.am
include doc/doxygen-dev/utils/Makefile.top.inc.am
include doc/doxygen-user/manual/Makefile.top.inc.am
include doc/doxygen-user/manpages/Makefile.top.inc.am
include doc/doxygen-user/pearl/Makefile.top.inc.am
include doc/patterns/Makefile.top.inc.am
include doc/quick-reference/Makefile.top.inc.am
include share/Makefile.top.inc.am
include src/pearl/example/Makefile.top.inc.am
include src/scout/generator/Makefile.top.inc.am
include src/utils/Makefile.top.inc.am


.PHONY: $(PHONY_TARGETS)
