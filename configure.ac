AC_INIT([clutterpad], [0.1], [mirsal.ennaime@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC
AC_PROG_INSTALL

AC_HEADER_STDC

PKG_CHECK_MODULES([GLIB], [glib-2.0 gthread-2.0],[
  CFLAGS="$GLIB_CFLAGS $CFLAGS"
  LIBS="$GLIB_LIBS $LIBS"
])

PKG_CHECK_MODULES([CLUTTER], [clutter-1.0 >= 1.0.6],[
  CFLAGS="$CLUTTER_CFLAGS $CFLAGS"
  LIBS="$CLUTTER_LIBS $LIBS"
])

AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
  Makefile
  src/Makefile
])

AC_OUTPUT
