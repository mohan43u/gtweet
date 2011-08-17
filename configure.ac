#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([tweetpts], [0.1], [mohan43u@gmail.com], [], [http://github.com/mohan43u/tweetpts])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([curl.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for modules
PKG_CHECK_MODULES([glib], [glib-2.0], [], [AC_MSG_ERROR([libglib mot found])])
PKG_CHECK_MODULES([libcurl], [libcurl], [], [AC_MSG_ERROR([libcurl mot found])])
PKG_CHECK_MODULES([oauth], [oauth], [], [AC_MSG_ERROR([liboauth mot found])])
PKG_CHECK_MODULES([jsonglib], [json-glib-1.0], [], [AC_MSG_ERROR([libjson-glib-1.0 mot found])])

# Checks for header files.
AC_CHECK_HEADER([stdlib.h], [], [AC_MSG_ERROR([stdlib.h not includable])])
AC_CHECK_HEADER([readline/readline.h], [], [AC_MSG_ERROR([readline/readline.h not includable])])
AC_CHECK_HEADER([wordexp.h], [], [AC_MSG_ERROR([wordexp.h not includable])])

# Checks for libraries
AC_CHECK_LIB([readline], [readline], [], [AC_MSG_ERROR([libreadline not available])])

# Checks for functions
AC_CHECK_FUNC([wordexp], [], [AC_MSG_ERROR([wordexp() function not available])])

# outputs
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
