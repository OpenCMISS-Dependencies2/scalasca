##*************************************************************************##
##  CUBE        http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2021                                                ##
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


## The SUBDIRS variable holds a list of subdirectories automake (recursivly)
## must build.
SUBDIRS = $(subdirs) $(ax_config_subdirs)

## ACLOCAL_AMFLAGS contains options to pass to aclocal when aclocal.m4 is to be
## rebuilt by make. This line is also used by autoreconf to run aclocal with
## suitable options, or by autopoint and gettextize to locate the place where
## Gettext's macros should be installed. So even if you do not really care about
## the rebuild rules, you should define ACLOCAL_AMFLAGS.
ACLOCAL_AMFLAGS = -I build-config/m4 -I build-config/common/m4 

PHONY_TARGETS =





if PLATFORM_MIC
AM_TARGET=/mic
else
AM_TARGET=
endif

 

include build-config/common/Makefile.inc.am
include vendor/windows/Makefile.win32.inc.am
include build-config/platforms/platforms.am
include build-config/common/platforms.am


## To install configuration summary file(s)
pkgdata_DATA = \
    @PACKAGE@.summary

## To install extra documentation files
dist_doc_DATA = \
    README \
    OPEN_ISSUES

## To clean up configuration summary files
DISTCLEANFILES += \
    $(builddir)/config.summary \
    $(builddir)/@PACKAGE@.summary


## Still, sometimes there are files that must be distributed, but which are not
## covered in the automatic rules. These files should be listed in the 
## EXTRA_DIST variable. You can mention files from subdirectories in EXTRA_DIST.
## 
## You can also mention a directory in EXTRA_DIST. In this case the entire
## directory will be recursively copied into the distribution. Please note that
## this will also copy everything in the directory, including CVS/RCS version
## control files. We recommend against using this feature.
EXTRA_DIST += $(srcdir)/src/config.h.in \
	      $(srcdir)/src/cubew-version.h.in \
              $(srcdir)/build-config/VERSION \
              $(srcdir)/build-config/REVISION \
	      $(srcdir)/build-config/common/generate-library-version.sh \
	      $(srcdir)/build-config/common/generate-package-version.sh \
	      $(srcdir)/share/@PACKAGE@/* 
	      




CLEANFILES += \
    src/cubew_config_usage.h 

BUILT_SOURCES += \
    src/cubew_config_usage.h 

src/cubew_config_usage.h: $(srcdir)/src//tools/cubew_config/cubew_config_usage.dox.in
	$(AM_V_GEN)$(AWK) '{print "\"" $$0 "\\n\""}' $< >$@






dist-hook: \
    doxygen-user-pack


clean-local: \
    doxygen-dev-clean \
    doxygen-user-clean




if ! CROSS_BUILD
EXTRA_DIST += $(srcdir)/build-frontend/configure.ac \
              $(srcdir)/build-frontend/aclocal.m4   \
              $(srcdir)/build-frontend/Makefile.am  \
              $(srcdir)/build-frontend/Makefile.in  \
              $(srcdir)/build-frontend/configure    \
              $(srcdir)/src/cubew-config-frontend.h.in
endif


CLEANFILES += 
DISTCLEANFILES +=\
		$(builddir)/config.summary \
                $(builddir)/@PACKAGE@.summary \
		include/cubew/cubew_error_decls.gen.h
uninstall-local:
	rm -rf $(DESTDIR)$(includedir)/cubew
	rm -rf $(DESTDIR)$(datadir)/@PACKAGE_NAME@
	rm -rf $(DESTDIR)$(datadir)/@PACKAGE@
	rm -rf $(DESTDIR)$(datadir)/doc/@PACKAGE@
	rm -f  $(DESTDIR)$(exec_prefix)/bin/cubew-config 

install-exec-local:
#	$(INSTALL) bin/cubew-config $(DESTDIR)$(exec_prefix)/bin/

install-data-local:  doxygen-user-unpack
	@echo "Install version information "
	$(MKDIR_P) $(DESTDIR)$(datadir)/@PACKAGE@$(AM_TARGET)
	@$(INSTALL_DATA) src/cubew-version.h  $(DESTDIR)$(includedir)/cubew/cubew-version.h
	@$(INSTALL_DATA) @PACKAGE@.summary $(DESTDIR)$(datadir)/@PACKAGE@$(AM_TARGET)/@PACKAGE@.summary
	@echo "=============================================================="
	$(MKDIR_P) $(DESTDIR)$(datadir)/doc/@PACKAGE@
	@echo "=============================================================="


 
if GIT_CONTROLLED

update-revision-file:
	@$(am__tty_colors); \
	if $(AM_V_P); then echo "cd $(srcdir) && git describe --long --always --dirty | $(SED) 's/.*-g//' >'build-config/REVISION'"; \
	              else echo "  GEN      $${mgn}$(srcdir)/build-config/REVISION$$std"; fi; \
	$(am__cd) $(srcdir) && git describe --long --always --dirty | $(SED) 's/.*-g//' >"build-config/REVISION"

else !GIT_CONTROLLED

update-revision-file:

endif !GIT_CONTROLLED



##--- Doxygen-related targets -----------------------------------------------

if GIT_CONTROLLED

## Documentation tarballs will not be generated in SVN mode,
## so there is nothing to unpack here...
doxygen-user-unpack:

else !GIT_CONTROLLED

htmldocpackdir	 = $(srcdir)/doc
doxygen-user-unpack:
	$(MKDIR_P) $(DESTDIR)$(docdir)
	for i in $(htmldocpackdir)/*-html.tar.gz; do \
		test ! -f $$i || tar xzvf $$i -C $(DESTDIR)$(docdir) || exit $$?; \
	done
	$(INSTALL_DATA) $(srcdir)/doc/*.pdf $(DESTDIR)$(docdir)

endif !GIT_CONTROLLED

##--- Include Makefile.am snippets ------------------------------------------

include doc/Makefile.top.inc.am
include doc/doxygen-common/Makefile.top.inc.am
include doc/doxygen-dev/cubew/Makefile.top.inc.am
include doc/doxygen-user/tools/Makefile.top.inc.am



.PHONY: $(PHONY_TARGETS)
	
# On MIC, install config.summary also in $pkgdatadir/mic
if PLATFORM_MIC

micpkgdatadir = $(pkgdatadir)/mic
micpkgdata_DATA = @PACKAGE@.summary

else !PLATFORM_MIC

TARGETS = mic

endif !PLATFORM_MIC
	
	
	
	
	
