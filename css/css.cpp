#include "css.hpp"

#include <antlr3.h>
#include "parser/css3.h"

static void parse(CSS * css, pANTLR3_INPUT_STREAM input);

CSS::CSS(const std::string &filename) : m_filename(filename) {
}

CSS::~CSS() { }

CSS * CSS::from_source(const std::string &source) {
	pANTLR3_INPUT_STREAM input = antlr3StringStreamNew((pANTLR3_UINT8) source.c_str(),ANTLR3_ENC_UTF8, (ANTLR3_UINT32)source.size()+1, (pANTLR3_UINT8)"inline");
	CSS * css = new CSS("inline");
	parse(css, input);
}

CSS * CSS::from_file(const std::string &filename) {
	pANTLR3_INPUT_STREAM input = antlr3FileStreamNew((pANTLR3_UINT8)filename.c_str(), ANTLR3_ENC_UTF8);
	CSS * css = new CSS(filename);
	parse(css, input);
}

static void parse(CSS * css, pANTLR3_INPUT_STREAM input) {
	pcss3Lexer lxr;
	pANTLR3_COMMON_TOKEN_STREAM tstream;
	pcss3Parser psr;
	css3Parser_stylesheet_return cssAST;

	pANTLR3_COMMON_TREE_NODE_STREAM nodes;

	//pcss3DeclDump treeDecl;

	lxr = css3LexerNew(input);

	if(lxr == nullptr) {
		fprintf(stderr,"Failed to create lexer\n");
		abort();
	}

	tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lxr));

	if(tstream == nullptr) {
		fprintf(stderr,"Failed to create token stream\n");
		abort();
	}

	psr = css3ParserNew(tstream);
	if(psr == nullptr) {
		fprintf(stderr,"Failed to create parser\n");
		abort();
	}

	cssAST = psr->stylesheet(psr);

	unsigned int errors = psr->pParser->rec->getNumberOfSyntaxErrors(psr->pParser->rec);

	if(errors > 0 ) {
		fprintf(stderr, "Parser returned %d errors\n", errors);
		abort();
	}

	pANTLR3_STRING tree = cssAST.tree->toStringTree(cssAST.tree);
	printf("Tree: %s\n", tree->chars);
}
