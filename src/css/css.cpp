#include <antlr3.h>

// ANTLR3 is retarded and includes it's automake config in public headers.
// Undefine that shit here:

#undef VERSION
#undef PACKAGE
#undef PACKAGE_BUGREPORT
#undef PACKAGE_STRING
#undef PACKAGE_NAME
#undef PACKAGE_TARNAME
#undef PACKAGE_URL
#undef PACKAGE_VERSION

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/css/css.hpp>

#include <derpkit/utils/string_utils.hpp>
#include <derpkit/dom/node.hpp>

#include "parser/css3.h"

#include <cassert>
#include <functional>
#include <sstream>

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

static void parse_input(pANTLR3_INPUT_STREAM input, const std::function<void(pcss3Parser)>& func) {
	pcss3Lexer lxr;
	pANTLR3_COMMON_TOKEN_STREAM tstream;
	pcss3Parser psr;

	lxr = css3LexerNew(input);

	if(lxr == nullptr) {
		fprintf(stderr,"Failed to create lexer\n");
	}

	tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lxr));

	if(tstream == nullptr) {
		fprintf(stderr,"Failed to create token stream\n");
	}

	psr = css3ParserNew(tstream);
	if(psr == nullptr) {
		fprintf(stderr,"Failed to create parser\n");
	}

	func(psr);

	psr->free(psr);
	tstream->free(tstream);
	lxr->free(lxr);
}

CSS * CSS::from_source(const std::string &source) {
	pANTLR3_INPUT_STREAM input = antlr3StringStreamNew((pANTLR3_UINT8) source.c_str(), ANTLR3_ENC_UTF8, (ANTLR3_UINT32)source.size(), (pANTLR3_UINT8)"inline");
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

void CSS::from_source_to_selector(const std::string& source, Selector& out) {
	pANTLR3_INPUT_STREAM input = antlr3StringStreamNew((pANTLR3_UINT8) source.c_str(), ANTLR3_ENC_UTF8, (ANTLR3_UINT32)source.size(), (pANTLR3_UINT8)"inline-selector");
	parse_input(input, [&out](pcss3Parser psr) {
		css3Parser_selector_return selectorAST = psr->selector(psr);

		unsigned int errors = psr->pParser->rec->getNumberOfSyntaxErrors(psr->pParser->rec);

		if(errors > 0 ) {
			fprintf(stderr, "Parser returned %d errors\n", errors);
		}

		pANTLR3_BASE_TREE node = selectorAST.tree;
		pANTLR3_COMMON_TOKEN token = node->getToken(node);
		if(token != NULL && token->getType(token) == CSS_SELECTOR) {
			parse_selector(node, out);
		} else {
			fprintf(stderr, "Invalid selector, got tree\n");
			abort();
		}
	});
	input->free(input);
}

void CSS::parse(pANTLR3_INPUT_STREAM input) {
	parse_input(input, [&](pcss3Parser psr) {
		css3Parser_stylesheet_return cssAST = psr->stylesheet(psr);

		unsigned int errors = psr->pParser->rec->getNumberOfSyntaxErrors(psr->pParser->rec);

		if(errors > 0 ) {
			fprintf(stderr, "Parser returned %d errors\n", errors);
		}

		traverse(cssAST.tree);
	});
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

void CSS::parse_selector(pANTLR3_BASE_TREE tree, Selector& selector) {
	traverse_tree(tree, [&selector](pANTLR3_BASE_TREE node) {
		pANTLR3_COMMON_TOKEN token = node->getToken(node);
		if(token->getType(token) == CSS_SELECTOR_UNIT) {
			selector.m_units.emplace_back();
			SelectorUnit& unit = selector.m_units.back();
			traverse_tree(node, [&unit](pANTLR3_BASE_TREE sel_node) {
				pANTLR3_COMMON_TOKEN sel_token = sel_node->getToken(sel_node);
				SelectorType type = convert_antlr_selector_type(sel_token->getType(sel_token));
				if(type == TYPE_UNKNOWN) {
					printf("[CSS] Warning: Unknown selector type %s\n",
						sel_token->getText(sel_token)->chars);
				} else {
					pANTLR3_BASE_TREE val_node = get_child(sel_node, 0);
					pANTLR3_COMMON_TOKEN val_token = val_node->getToken(val_node);
					std::string value = str_trim(convert_string(val_token->getText(val_token)));

					unit.m_atoms.emplace_back(type, value);
				}
			});
		} else if(token->getType(token) == CSS_SELECTOR_COMBINATOR) {
			pANTLR3_BASE_TREE cnode = get_child(node, 0);
			pANTLR3_COMMON_TOKEN ctoken = cnode->getToken(cnode);
			if(selector.m_units.size() > 0) {
				SelectorUnit& unit = selector.m_units.back();
				std::string combinator = str_trim(convert_string(ctoken->getText(ctoken)));
				if(combinator.length() > 0) {
					switch(combinator[0]) {
						case '>':
							unit.combinator = COMBINATOR_CHILD;
							break;
						case '~':
							unit.combinator = COMBINATOR_GENERAL_SIBLING;
							break;
						case '+':
							unit.combinator = COMBINATOR_ADJACENT_SIBLING;
							break;
						default:
							printf("[CSS] Error: Unknown combinator: %s\n", ctoken->getText(ctoken)->chars);
					}
				} else {
					// Combinator was space (and was trimmed)
					unit.combinator = COMBINATOR_DESCENDANT;
				}
			} else {
				printf("[CSS] Error: Combinator before any selector: %s\n", ctoken->getText(ctoken)->chars);
			}
		} else {
			printf("[CSS] Error: Unexpected token when parsing selector, got %s\n", token->getText(token)->chars);
		}

	});

	if(selector.m_units.size() > 0) selector.calculate_specificity();
}

void CSS::parse_rule(pANTLR3_BASE_TREE node) {
	m_rules.emplace_back();
	Rule& rule = m_rules.back();
	traverse_tree(node, [&rule, this](pANTLR3_BASE_TREE node) {
		pANTLR3_COMMON_TOKEN token = node->getToken(node);
		if(token != NULL) {
			switch(token->getType(token)) {
				case CSS_SELECTOR:
				{
					rule.m_selectors.emplace_back();
					Selector& selector = rule.m_selectors.back();
					parse_selector(node, selector);
					if(selector.m_units.size() == 0) rule.m_selectors.pop_back();
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
								value << str_trim(convert_string(prop_token->getText(prop_token))) << " ";
								break;
						}
					}
					if(!value.str().empty()) {
						property.value = lcase(str_trim(value.str()));
						rule.m_properties.push_back(property);
					}
				}
				break;
			}
		}
	});
}

void CSS::apply_to_document(dom::Document& doc) const {
	for(const auto& rule : rules()) {
		const std::vector<Property>& properties = rule.properties();
		for(const auto& selector : rule.selectors()) {
			std::vector<dom::Node> nodes = doc.find(selector);
			if(nodes.empty()) continue;

			for(const auto& property : properties) {
				Specificity specificity = selector.specificity();
				specificity.important = property.important ? 1 : 0;
				for(auto& node : nodes) {
					std::map<std::string,dom::NodeCSSProperty>& node_properties = node.css_properties();
					auto it = node_properties.find(property.property);
					if(it != node_properties.end()) {
						if(!(it->second.specificity < specificity)) {
							it->second = dom::NodeCSSProperty(property.value, specificity);
						}
					} else {
						node_properties.emplace(property.property, dom::NodeCSSProperty(property.value, specificity));
					}
				}
			}
		}
	}
}

void CSS::print() const {
	for(const auto & r : m_rules) {
		r.print();
	}
}

}
