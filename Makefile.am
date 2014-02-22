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
css_libparser_a_SOURCES = \
	src/css/parser/css3.g \
	src/css/parser/css3Parser.c \
	src/css/parser/css3Lexer.c

libderpkit_la_CXXFLAGS = ${AM_CXXFLAGS} -Isrc/css/parser
libderpkit_la_LIBADD = css/libparser.a
libderpkit_la_LIBS = -static -lantlr3c
libderpkit_la_SOURCES = \
	src/css/css.cpp src/css/css.hpp \
	src/css/cssproperty.cpp src/css/cssproperty.hpp \
	src/css/cssrule.cpp src/css/cssrule.hpp \
	src/css/cssselector.cpp src/css/cssselector.hpp

parse_LIBS = libderpkit.la
parse_SOURCES = src/main.cpp

src/css/parser/css3Parser.c: src/css/parser/css3.g
	@test -e src/css/parser || mkdir -p src/css/parser
	antlr3 -make -fo src/css/parser $<
