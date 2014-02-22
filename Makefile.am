ACLOCAL_AMFLAGS = -I m4
AM_DEFAULT_SOURCE_EXT = .cpp

AM_LDFLAGS = -pthread
AM_CXXFLAGS = -Wall
AM_CFLAGS = -Wall

lib_LTLIBRARIES = libderpkit.la
noinst_LIBRARIES = css/libparser.a
noinst_PROGRAMS = parse

# ANTLR generates code which exhibits warnings, must compile with different flags
css_libparser_a_CFLAGS = 
css_libparser_a_SOURCES = css/parser/css3Parser.c css/parser/css3Lexer.c

libderpkit_la_CXXFLAGS = ${AM_CXXFLAGS} -Icss/parser
libderpkit_la_LIBADD = css/libparser.a
libderpkit_la_LIBS = -static -lantlr3c
libderpkit_la_SOURCES = \
	css/css.cpp css/css.hpp \
	css/cssproperty.cpp css/cssproperty.hpp \
	css/cssrule.cpp css/cssrule.hpp \
	css/cssselector.cpp css/cssselector.hpp

parse_LIBS = libderpkit.la
parse_SOURCES = main.cpp

css/parser/css3Parser.c: ${top_srcdir}/css/parser/css3.g
	@test -e css/parser || mkdir -p css/parser
	antlr3 -make -fo css/parser ${top_srcdir}/css/parser/css3.g
