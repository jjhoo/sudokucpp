# -*- Autoconf -*-

AC_PREREQ([2.69])
AC_INIT([libsudokucpp], [0.0.1], [jjhakala@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
LT_INIT([disable-fast-install, win32-dll])

AC_CONFIG_SRCDIR([solver.cpp])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CXX
AC_PROG_CC

CXXFLAGS='-O2 -Wall -std=c++14'

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
])
AC_OUTPUT
