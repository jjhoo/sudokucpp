AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libsudokucpp.la
bin_PROGRAMS = test

libsudokucpp_la_SOURCES = \
	eliminators.cpp \
	solver.cpp

libsudokucpp_la_includedir = .
libsudokucpp_la_include_HEADERS = \
	combinations.h \
	permutations.h

test_SOURCES = test.cpp
test_LDADD = libsudokucpp.la
