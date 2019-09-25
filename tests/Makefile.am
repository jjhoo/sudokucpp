AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/googletest/include
check_PROGRAMS = sudoku_test
TESTS = sudoku_test

sudoku_test_SOURCES = sudoku_test.cpp
sudoku_test_LDADD = ../sudokucpp/libsudokucpp.la ../googletest/lib/libgtest.a -lpthread
