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
## Copyright (c) 2009-2014, 2019-2020,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2013,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2015, 2017,
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

## file       Makefile.am


## The SUBDIRS variable holds a list of subdirectories automake (recursivly)
## must build.
## first build in the top level, so that we can check for template updates
SUBDIRS = . $(subdirs) $(ax_config_subdirs)

## Options passed to aclocal to specify local macro directories. Although
## ACLOCAL_AMFLAGS should be replaced by AC_CONFIG_MACRO_DIRS, aclocal-1.13.4
## doesn't honor the paths provided to AC_CONFIG_MACRO_DIRS. See
## https://debbugs.gnu.org/cgi/bugreport.cgi?bug=21352
ACLOCAL_AMFLAGS = -I build-config/m4 -I build-config/common/m4

PYTHON = $(PYTHON_FOR_GENERATOR)

CLEAN_LOCAL =
clean-local: $(CLEAN_LOCAL)

CHECK_LOCAL =
check-local: $(CHECK_LOCAL)

DIST_HOOK =
dist-hook: $(DIST_HOOK)

include build-config/common/Makefile.inc.am
include build-config/common/platforms.am

## Still, sometimes there are files that must be distributed, but which are not
## covered in the automatic rules. These files should be listed in the
## EXTRA_DIST variable. You can mention files from subdirectories in EXTRA_DIST.
##
## You can also mention a directory in EXTRA_DIST. In this case the entire
## directory will be recursively copied into the distribution. Please note that
## this will also copy everything in the directory, including CVS/RCS version
## control files. We recommend against using this feature.
EXTRA_DIST += $(srcdir)/src/config.h                              \
              $(srcdir)/src/config-custom.h                       \
              $(srcdir)/otf2-mini-config.sh.in                     \
              $(srcdir)/otf2-mini-config-data.sh.in                \
              $(srcdir)/doc/doxygen-user/A1_installation.dox      \
              $(srcdir)/doc/doxygen-user/A2_io.dox                \
              $(srcdir)/doc/doxygen-user/definitions.dox          \
              $(srcdir)/doc/doxygen-user/events.dox               \
              $(srcdir)/doc/doxygen-user/snaps.dox                \
              $(srcdir)/doc/doxygen-user/markers.dox \
              $(srcdir)/doc/doxygen-user/otf2_config_usage.dox.in \
              $(srcdir)/doc/doxygen-user/otf2_print_usage.dox.in  \
              $(srcdir)/doc/doxygen-user/otf2_marker_usage.dox.in \
              $(srcdir)/doc/doxygen-user/otf2_snapshots_usage.dox.in \
              $(srcdir)/doc/doxygen-user/otf2_estimator_usage.dox.in \
              $(srcdir)/OPEN_ISSUES \
              $(srcdir)/THANKS

doc_DATA = $(srcdir)/OPEN_ISSUES \
           $(srcdir)/ChangeLog

if ! CROSS_BUILD
EXTRA_DIST += $(srcdir)/build-frontend/configure.ac \
              $(srcdir)/build-frontend/aclocal.m4   \
              $(srcdir)/build-frontend/Makefile.am  \
              $(srcdir)/build-frontend/Makefile.in  \
              $(srcdir)/build-frontend/configure    \
              $(srcdir)/src/config-frontend.h.in
endif

CLEANFILES += \
    src/otf2_print_usage.h \
    src/otf2_config_usage.h \
    src/otf2_snapshots_usage.h \
    src/otf2_estimator_usage.h \
    src/otf2_marker_usage.h

BUILT_SOURCES += \
    otf2-mini-config.sh \
    src/otf2_print_usage.h \
    src/otf2_config_usage.h \
    src/otf2_snapshots_usage.h \
    src/otf2_estimator_usage.h \
    src/otf2_marker_usage.h

src/otf2_print_usage.h: $(srcdir)/doc/doxygen-user/otf2_print_usage.dox.in
	$(AM_V_GEN)$(AWK) '{print "\"" $$0 "\\n\""}' $< >$@

src/otf2_config_usage.h: $(srcdir)/doc/doxygen-user/otf2_config_usage.dox.in
	$(AM_V_GEN)$(AWK) '{print "\"" $$0 "\\n\""}' $< >$@

src/otf2_snapshots_usage.h: $(srcdir)/doc/doxygen-user/otf2_snapshots_usage.dox.in
	$(AM_V_GEN)$(AWK) '{print "\"" $$0 "\\n\""}' $< >$@

src/otf2_estimator_usage.h: $(srcdir)/doc/doxygen-user/otf2_estimator_usage.dox.in
	$(AM_V_GEN)$(AWK) '{print "\"" $$0 "\\n\""}' $< >$@

src/otf2_marker_usage.h: $(srcdir)/doc/doxygen-user/otf2_marker_usage.dox.in
	$(AM_V_GEN)$(AWK) '{print "\"" $$0 "\\n\""}' $< >$@

install-data-local: doxygen-user-install

install-data-hook:
	$(AM_V_at)for target in $(TARGETS)""; do \
	    if test -n "$$target" && test -f $(pkgdatadir)/$$target/@PACKAGE@.summary; then \
	        echo >> $(DESTDIR)$(pkgdatadir)/@PACKAGE@.summary ; \
	        echo >> $(DESTDIR)$(pkgdatadir)/@PACKAGE@.summary ; \
	        echo "=====[ Target: $$target ]====================" >> $(DESTDIR)$(pkgdatadir)/@PACKAGE@.summary ; \
	        echo >> $(DESTDIR)$(pkgdatadir)/@PACKAGE@.summary ; \
	        cat $(pkgdatadir)/$$target/@PACKAGE@.summary >> $(DESTDIR)$(pkgdatadir)/@PACKAGE@.summary ; \
	    fi \
	done

uninstall-hook: doxygen-uninstall common-uninstall

nodist_bin_SCRIPTS=
pkgdata_DATA=@PACKAGE@.summary
noinst_DATA=

DISTCLEANFILES += \
    include/otf2/OTF2_ErrorCodes.h \
    src/otf2_error_decls.gen.h

BUILT_SOURCES += \
    include/otf2/OTF2_ErrorCodes.h \
    src/otf2_error_decls.gen.h

include/otf2/OTF2_ErrorCodes.h: $(srcdir)/share/otf2/otf2.errors
src/otf2_error_decls.gen.h: $(srcdir)/share/otf2/otf2.errors

include include/otf2/Makefile.inc.am

include share/Makefile.inc.am
include src/tools/otf2_template/Makefile.inc.am

include templates/Makefile.inc.am

include doc/Makefile.inc.am
include doc/examples/Makefile.inc.am

include contrib-build-vs/Makefile.inc.am

# On MIC, install config.summary also in $pkgdatadir/mic
if PLATFORM_MIC

micpkgdatadir = $(pkgdatadir)/mic
micpkgdata_DATA = @PACKAGE@.summary

else !PLATFORM_MIC

TARGETS = mic

endif !PLATFORM_MIC

# non-color automake only provides $(am__tty_colors) if tests are in use
TESTS =
