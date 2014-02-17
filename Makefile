CFLAGS=-Icss/parser -I/usr/local/include -g
CXXFLAGS=-Icss/parser -I/usr/local/include -g -std=c++0x
LDFLAGS=-L/usr/local/lib
LIBS=--static -lantlr3c

all: parse

grammar: css/parser/css3.g
	antlr3 -make -fo css/parser css/parser/css3.g

css/css.o: css/css.hpp css/css.cpp
	g++ ${CXXFLAGS} -c css/css.cpp -o css/css.o

css/cssselector.o: css/cssselector.hpp css/cssselector.cpp
	g++ ${CXXFLAGS} -c css/cssselector.cpp -o css/cssselector.o

css/cssrule.o: css/cssrule.hpp css/cssrule.cpp
	g++ ${CXXFLAGS} -c css/cssrule.cpp -o css/cssrule.o

css/cssproperty.o: css/cssproperty.hpp css/cssproperty.cpp
	g++ ${CXXFLAGS} -c css/cssproperty.cpp -o css/cssproperty.o

main.o: main.cpp
	g++ ${CXXFLAGS} -c main.cpp -o main.o

parse: grammar css/parser/css3Parser.o css/parser/css3Lexer.o css/css.o css/cssrule.o css/cssselector.o css/cssproperty.o main.o
	g++ ${LDFLAGS} css/parser/css3Lexer.o css/parser/css3Parser.o css/css.o css/cssrule.o css/cssproperty.o css/cssselector.o main.o ${LIBS} -o parse

clean:
	rm -f css/*.o css/parser/*.o *.o parse css/parse/css3Lexer.h css/parse/css3Lexer.c css/parse/css3Parser.h css/parse/css3Parser.c css/parser/css3.tokens
