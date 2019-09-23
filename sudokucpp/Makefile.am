AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libsudokucpp.la

libsudokucpp_la_SOURCES = \
	eliminators.cpp \
	solver.cpp \
	sudoku.cpp

libsudokucpp_la_CPPFLAGS = $(ACE_CFLAGS)
libsudokucpp_la_LIBADD = $(ACE_LIBS)
libsudokucpp_la_includedir = $(includedir)/sudokucpp
libsudokucpp_la_include_HEADERS = \
	combinations.h \
	eliminators.h \
	permutations.h \
	sudoku.h
