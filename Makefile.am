ACLOCAL_AMFLAGS = -I m4
AM_DEFAULT_SOURCE_EXT = .cpp

AM_LDFLAGS = -pthread
AM_CXXFLAGS = -Wall -fvisibility=hidden -I${top_srcdir}/include -I${top_srcdir}/src
AM_CFLAGS = ${AM_CXXFLAGS}

lib_LTLIBRARIES = libderpkit.la
noinst_LTLIBRARIES = libantlr3c.la
noinst_PROGRAMS = parse domtest

TESTS = test/dom
if BUILD_TESTS
check_PROGRAMS = ${TESTS}
endif

# ANTLR generates code which exhibits warnings, must compile with different flags
#src/css/parser/libderpkit_la-css3Parser.lo: CFLAGS += -Wno-unused
#src/css/parser/libderpkit_la-css3Lexer.lo:  CFLAGS += -Wno-unused

libderpkit_la_CXXFLAGS = ${AM_CXXFLAGS} -I${top_builddir}/src/css/parser -I${top_srcdir}/vendor/antlr3/include
libderpkit_la_CFLAGS = ${libderpkit_la_CXXFLAGS} -Wno-unused
libderpkit_la_LIBADD = libantlr3c.la
libderpkit_la_SOURCES = \
	include/derpkit/derpkit.hpp \
	include/derpkit/export.hpp \
	include/derpkit/forward.hpp \
	src/css/parser/css3.g \
	src/css/parser/css3Parser.c src/css/parser/css3Parser.h \
	src/css/parser/css3Lexer.c src/css/parser/css3Lexer.h \
	src/css/css.cpp src/css/css.hpp \
	src/css/expression.cpp src/css/expression.hpp \
	src/css/parsers.cpp src/css/parsers.hpp \
	src/css/property.cpp src/css/property.hpp \
	src/css/rule.cpp src/css/rule.hpp \
	src/css/selector.cpp src/css/selector.hpp \
	src/css/specificity.cpp src/css/specificity.hpp \
	include/derpkit/dom/document.hpp src/dom/document.cpp \
	include/derpkit/dom/node.hpp src/dom/node.cpp \
	include/derpkit/utils/string.hpp src/utils/string.cpp \
	include/derpkit/utils/string_utils.hpp src/utils/string_utils.cpp \
	src/version.cpp include/derpkit/version.hpp

libantlr3c_la_CFLAGS  = -fvisibility=hidden -w -I${top_srcdir}/vendor/antlr3/include
libantlr3c_la_SOURCES	=	vendor/antlr3/src/antlr3baserecognizer.c		\
							vendor/antlr3/src/antlr3basetree.c				\
							vendor/antlr3/src/antlr3basetreeadaptor.c		\
							vendor/antlr3/src/antlr3bitset.c				\
							vendor/antlr3/src/antlr3collections.c			\
							vendor/antlr3/src/antlr3commontoken.c			\
							vendor/antlr3/src/antlr3commontree.c			\
							vendor/antlr3/src/antlr3commontreeadaptor.c		\
							vendor/antlr3/src/antlr3commontreenodestream.c	\
							vendor/antlr3/src/antlr3convertutf.c			\
							vendor/antlr3/src/antlr3cyclicdfa.c				\
							vendor/antlr3/src/antlr3debughandlers.c			\
							vendor/antlr3/src/antlr3encodings.c				\
							vendor/antlr3/src/antlr3exception.c				\
							vendor/antlr3/src/antlr3filestream.c			\
							vendor/antlr3/src/antlr3inputstream.c			\
							vendor/antlr3/src/antlr3intstream.c				\
							vendor/antlr3/src/antlr3lexer.c					\
							vendor/antlr3/src/antlr3parser.c				\
							vendor/antlr3/src/antlr3rewritestreams.c		\
							vendor/antlr3/src/antlr3string.c				\
							vendor/antlr3/src/antlr3tokenstream.c			\
							vendor/antlr3/src/antlr3treeparser.c

parse_CXXFLAGS = ${AM_CXXFLAGS} -I${top_srcdir}/include
parse_LDADD = libderpkit.la
parse_SOURCES = src/main.cpp

domtest_CXXFLAGS = ${AM_CXXFLAGS} -I${top_srcdir}/include -I${top_srcdir}/src
domtest_LDADD = libderpkit.la
domtest_SOURCES = src/dom/test.cpp

test_dom_CXXFLAGS = ${AM_CXXFLAGS} $(CPPUNIT_CFLAGS)
test_dom_LDADD = libderpkit.la $(CPPUNIT_LIBS)

CLEANFILES = ${top_srcdir}/src/css/parser/css3Lexer.c \
			 ${top_srcdir}/src/css/parser/css3Lexer.h \
			 ${top_srcdir}/src/css/parser/css3Parser.c \
			 ${top_srcdir}/src/css/parser/css3Parser.h

${top_srcdir}/src/css/parser/css3Parser.c: src/css/parser/css3Lexer.c
${top_srcdir}/src/css/parser/css3Parser.h: src/css/parser/css3Lexer.c
${top_srcdir}/src/css/parser/css3Lexer.h: src/css/parser/css3Lexer.c
${top_srcdir}/src/css/parser/css3Lexer.c: src/css/parser/css3.g
	@test -e ${top_srcdir}/src/css/parser || mkdir -p ${top_srcdir}/src/css/parser
	antlr3 -make -fo ${top_srcdir}/src/css/parser $<

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libderpkit.pc
