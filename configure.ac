AC_PREREQ([2.64])
AC_INIT([dingo], [0.5.0], [http://dingo-project.org/contribute/file-bugs])
AC_CONFIG_SRCDIR([src/dingo.cc])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11])

# Include m4 macros
m4_include([m4/ax_boost_base.m4])

# Checks for programs
AC_PROG_CXX
AC_PROG_INSTALL

IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=dingo
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"],
                   [The domain to use with gettext])

DINGO_LOCALEDIR=[${datadir}/locale]
AC_SUBST(DINGO_LOCALEDIR)

# Checks for libraries
# Initialize pkg-config
PKG_PROG_PKG_CONFIG([0.25])
AX_BOOST_BASE([1.48])
PKG_CHECK_MODULES([DEPENDENCIES], [gtkmm-3.0 gstreamermm-0.10 libxml++-2.6 libmediainfo sqlite3 libnotify])

AM_GLIB_GNU_GETTEXT

# Checks for header files
AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([stdio.h stdlib.h ctype.h stdint.h gtkmm.h gstreamermm.h glibmm/i18n.h sqlite3.h MediaInfo/MediaInfo.h gtkmm/volumebutton.h gdk/gdkx.h libxml++/libxml++.h libintl.h boost/program_options.hpp])

# Checks for typedefs, structures, and compiler characteristics
# N/A

# Checks for library functions
# N/A

AC_CONFIG_FILES([Makefile data/Makefile docs/Makefile build/Makefile])
AC_CONFIG_FILES([po/Makefile.in src/Makefile])
AC_CONFIG_FILES([data/database/Makefile data/icons/Makefile])
AC_CONFIG_FILES([data/samples/Makefile data/settings/Makefile])
AC_CONFIG_FILES([data/icons/16x16/Makefile data/icons/24x24/Makefile])
AC_CONFIG_FILES([data/icons/32x32/Makefile data/icons/48x48/Makefile])
AC_CONFIG_FILES([data/icons/128x128/Makefile data/icons/256x256/Makefile])
AC_CONFIG_FILES([data/icons/scalable/Makefile data/integration/Makefile])
AC_CONFIG_FILES([data/integration/dingo.desktop.in])
AC_CONFIG_FILES([build/rpms/Makefile build/debs/Makefile])

AC_OUTPUT
