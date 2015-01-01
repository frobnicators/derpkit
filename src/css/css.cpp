#include "css.hpp"

#include <antlr3.h>
#include <cassert>
#include <functional>
#include <sstream>
#include "parser/css3.h"

namespace css {

static SelectorType convert_antlr_selector_type(ANTLR3_UINT32 type) {
	switch(type) {
		case CSS_TAG: return TYPE_TAG;
		case CSS_CLASS: return TYPE_CLASS;
		case CSS_ID: return TYPE_ID;
		case CSS_PSEUDO: return TYPE_PSEUDO;
		case CSS_ANY: return TYPE_ANY;
		default: return TYPE_UNKNOWN;
	}
}

static pANTLR3_BASE_TREE get_child_or_null(pANTLR3_BASE_TREE tree, unsigned int index) {
	if(tree->getChildCount(tree) > index) {
		return (pANTLR3_BASE_TREE)tree->getChild(tree, index);
	} else {
		return nullptr;
	}
}

static pANTLR3_BASE_TREE get_child(pANTLR3_BASE_TREE tree, unsigned int index) {
	pANTLR3_BASE_TREE n = get_child_or_null(tree, index);
	assert(n != nullptr);
	return n;
}

static void traverse_tree(pANTLR3_BASE_TREE node, std::function<void(pANTLR3_BASE_TREE node)> callback) {
	for(unsigned int c=0; c<node->getChildCount(node); ++c) {
		callback(get_child(node, c));
	}
}

static std::string convert_string(pANTLR3_STRING str) {
	return std::string((const char*)str->chars);
}

CSS::CSS(const std::string &filename) : m_filename(filename) { }

CSS::~CSS() { }

CSS * CSS::from_source(const std::string &source) {
	pANTLR3_INPUT_STREAM input = antlr3StringStreamNew((pANTLR3_UINT8) source.c_str(), ANTLR3_ENC_UTF8, (ANTLR3_UINT32)source.size()+1, (pANTLR3_UINT8)"inline");
	CSS * css = new CSS("inline");
	css->parse(input);
	return css;
}

CSS * CSS::from_file(const std::string &filename) {
	pANTLR3_INPUT_STREAM input = antlr3FileStreamNew((pANTLR3_UINT8)filename.c_str(), ANTLR3_ENC_UTF8);
	CSS * css = new CSS(filename);
	css->parse(input);
	return css;
}

void CSS::parse(pANTLR3_INPUT_STREAM input) {
	pcss3Lexer lxr;
	pANTLR3_COMMON_TOKEN_STREAM tstream;
	pcss3Parser psr;
	css3Parser_stylesheet_return cssAST;

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

	traverse(cssAST.tree);
}

void CSS::traverse(pANTLR3_BASE_TREE node) {
	pANTLR3_COMMON_TOKEN token = node->getToken(node);
	if(token != NULL) {
		switch(token->getType(token)) {
			case CSS_RULE:
				parse_rule(node);
				break;
			default:
				printf("[CSS] Warning: Unknown node %s\n", token->getText(token)->chars);

		}
	} else {
		traverse_tree(node, [&](pANTLR3_BASE_TREE node) { traverse(node); });
	}
}

void CSS::parse_rule(pANTLR3_BASE_TREE node) {
	m_rules.emplace_back();
	Rule& rule = m_rules.back();
	traverse_tree(node, [&rule](pANTLR3_BASE_TREE node) {
		pANTLR3_COMMON_TOKEN token = node->getToken(node);
		if(token != NULL) {
			switch(token->getType(token)) {
				case CSS_SELECTOR:
				{
					rule.m_selectors.emplace_back();
					Selector& selector = rule.m_selectors.back();

					traverse_tree(node, [&selector](pANTLR3_BASE_TREE sel_node) {
						pANTLR3_COMMON_TOKEN sel_token = sel_node->getToken(sel_node);
						SelectorType type = convert_antlr_selector_type(sel_token->getType(sel_token));
						if(type == TYPE_UNKNOWN) {
							printf("[CSS] Warning: Unknown selector type %s\n",
								sel_token->getText(sel_token)->chars);
						} else {
							pANTLR3_BASE_TREE val_node = get_child(sel_node, 0);
							pANTLR3_COMMON_TOKEN val_token = val_node->getToken(val_node);
							std::string value = convert_string(val_token->getText(val_token));

							selector.m_atoms.emplace_back(type, value);
						}
					});

					if(selector.m_atoms.size() == 0) rule.m_selectors.pop_back();
					else selector.calculate_specificity();
				}
				break;
				case CSS_PROPERTY:
				{
					pANTLR3_BASE_TREE prop_node = get_child(node, 0);
					pANTLR3_COMMON_TOKEN prop_token = prop_node->getToken(prop_node);
					Property property(convert_string(prop_token->getText(prop_token)));
					std::stringstream value;
					for(unsigned int i=1; i<node->getChildCount(node); ++i) {
						pANTLR3_BASE_TREE prop_node = get_child(node, i);
						pANTLR3_COMMON_TOKEN prop_token = prop_node->getToken(prop_node);
						switch(prop_token->getType(prop_token)) {
							case CSS_IMPORTANT:
								property.important = true;
								break;
							default:
								value << convert_string(prop_token->getText(prop_token)) << " ";
								break;
						}
					}
					if(!value.str().empty()) {
						property.value = value.str();
						rule.m_properties.push_back(property);
					}
				}
				break;
			}
		}
	});
}

void CSS::apply_to_tree(dom::Node root) {

}

void CSS::print() const {
	for(const auto & r : m_rules) {
		r.print();
	}
}

}
