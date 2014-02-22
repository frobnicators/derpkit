m4_define([_VERSION_MAJOR], [0])
m4_define([_VERSION_MINOR], [1])
m4_define([_VERSION_MICRO], [0])
m4_define([_VERSION_NUM], m4_join([.],_VERSION_MAJOR, _VERSION_MINOR, _VERSION_MICRO))
AC_INIT([derpkit],_VERSION_NUM,[https://github.com/frobnicators/derpkit/issues/new],,[https://github.com/frobnicators/derpkit])

AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall -Werror])
AM_SILENT_RULES([yes])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile libderpkit.pc])
AH_BOTTOM([
#include "derpkit/forward.hpp"
using namespace derpkit;
])

VERSION_MAJOR=_VERSION_MAJOR
VERSION_MINOR=_VERSION_MINOR
VERSION_MICRO=_VERSION_MICRO
AC_DEFINE_UNQUOTED([VERSION_MAJOR],  [${VERSION_MAJOR}], [Major version number of package])
AC_DEFINE_UNQUOTED([VERSION_MINOR],  [${VERSION_MINOR}], [Minor version number of package])
AC_DEFINE_UNQUOTED([VERSION_MICRO],  [${VERSION_MICRO}], [Micro version number of package])

AC_GNU_SOURCE
AC_USE_SYSTEM_EXTENSIONS
AM_PROG_CC_C_O
AC_PROG_CXX
AM_PROG_AR
LT_INIT

AC_LANG_PUSH([C++])
AX_CHECK_COMPILE_FLAG([-std=c++0x], [CXXFLAGS="$CXXFLAGS -std=c++0x"], [AC_MSG_ERROR([Support for C++11 is required])])
AC_LANG_POP

pkgdatadir="${datadir}/${PACKAGE}"
AC_SUBST([pkgdatadir])

AC_OUTPUT
