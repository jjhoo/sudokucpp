# -*- Autoconf -*-

AC_PREREQ([2.69])
AC_INIT([libsudokucpp], [0.0.1], [jjhakala@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
LT_INIT([disable-fast-install, win32-dll])

AC_CONFIG_SRCDIR([sudokucpp/solver.cpp])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CXX
AC_PROG_CC

AC_LANG([C])
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_14(noext, mandatory)

AC_LANG_PUSH([C++])
CXXFLAGS="-Wall -Werror -std=c++14"
AC_LANG_POP

AC_ARG_WITH([ace], AS_HELP_STRING([--with-ace], [Build with the ACE library]))
AS_IF([test x$with_ace = xyes], [
   PKG_CHECK_MODULES([ACE], [ACE >= 6.4.0],
                     [AC_DEFINE([HAVE_LIBACE], [1], [Use ACE])],
                     [AC_DEFINE([HAVE_LIBACE], [0], [Do not ACE])])
])

AC_CHECK_HEADERS([stdint.h])

AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT8_T

AC_HEADER_STDC

AC_CONFIG_FILES([
    Makefile
    sudokucpp/Makefile
    tests/Makefile
])
AC_CONFIG_SUBDIRS([googletest])
AC_OUTPUT
