ACLOCAL_AMFLAGS = -I m4
AM_DEFAULT_SOURCE_EXT = .cpp

AM_LDFLAGS = -pthread
AM_CXXFLAGS = -Wall -fvisibility=hidden
AM_CFLAGS = ${AM_CXXFLAGS}

lib_LTLIBRARIES = libderpkit.la
noinst_PROGRAMS = parse

# ANTLR generates code which exhibits warnings, must compile with different flags
src/css/parser/css3Parser.lo: CFLAGS += -Wno-unused
src/css/parser/css3Lexer.lo:  CFLAGS += -Wno-unused

libderpkit_la_CXXFLAGS = ${AM_CXXFLAGS} -I${top_srcdir}/include -Isrc/css/parser
libderpkit_la_LIBS = -static -lantlr3c
libderpkit_la_SOURCES = \
	include/derpkit/derpkit.hpp \
	include/derpkit/export.hpp \
	include/derpkit/forward.hpp \
	src/css/parser/css3.g \
	src/css/parser/css3Parser.c src/css/parser/css3Parser.h \
	src/css/parser/css3Lexer.c src/css/parser/css3Lexer.h \
	src/css/css.cpp src/css/css.hpp \
	src/css/cssproperty.cpp src/css/cssproperty.hpp \
	src/css/cssrule.cpp src/css/cssrule.hpp \
	src/css/cssselector.cpp src/css/cssselector.hpp \
	src/version.cpp include/derpkit/version.hpp

parse_CXXFLAGS = ${AM_CXXFLAGS} -I${top_srcdir}/include
parse_LDADD = libderpkit.la
parse_SOURCES = src/main.cpp

src/css/parser/css3Parser.c: src/css/parser/css3Lexer.c
src/css/parser/css3Parser.h: src/css/parser/css3Lexer.c
src/css/parser/css3Lexer.h: src/css/parser/css3Lexer.c
wsrc/css/parser/css3Lexer.c: src/css/parser/css3.g
	@test -e src/css/parser || mkdir -p src/css/parser
	antlr3 -make -fo src/css/parser $<

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libderpkit.pc
