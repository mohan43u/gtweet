#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([gtweet], [0.1], [mohan43u@gmail.com], [], [http://github.com/mohan43u/tweetpts/tree/gtweet])
AC_CONFIG_SRCDIR([lib/tweet-object.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([-Wportability])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
packages='gio-unix-2.0 gobject-2.0 gtk+-3.0 gdk-3.0 oauth gobject-introspection-1.0 libsoup-2.4'
AC_SUBST([packages], [$packages])
PKG_CHECK_MODULES([PKGCFG], [$packages])
CFLAGS='-O0 -ggdb'

# GObject Introspection
GOBJECT_INTROSPECTION_CHECK([1.30.0])

# Checks for header files.
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_HEADER_STDBOOL

# Checks for library functions.
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile
		 gtweet.pc
                 doc/Makefile
                 lib/Makefile
		 exec/Makefile])
AC_OUTPUT
