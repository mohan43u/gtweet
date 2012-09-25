#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([tweetpts], [0.2], [mohan43u@gmail.com], [], [http://github.com/mohan43u/tweetpts])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/tweetpts-readlineapi.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
PKG_CHECK_MODULES([PKGCFG], [glib-2.0 ncursesw panelw oauth libcurl json-glib-1.0])

# Checks for header files.
AC_CHECK_HEADERS([locale.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_HEADER_STDBOOL

# Checks for library functions.
AC_CHECK_FUNCS([memset setlocale])

AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 src/Makefile])
AC_OUTPUT
