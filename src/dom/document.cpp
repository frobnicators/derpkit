#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/dom/document.hpp>
#include <derpkit/css/css.hpp>
#include "state.hpp"
#include "css/parsers.hpp"
#include <cassert>
#include <cstddef>
#include <sstream>

static const int MAX_DEPTH = 200;

using css::parsers::INHERIT;
using css::parsers::NO_INHERIT;

namespace dom {

static void apply_css_to_state(Node node, State* state, const State* parent);

Document::Document(){

}

Node Document::create_element(const char* tag){
	return Node(tag);
}

Node Document::create_element(const char* tag, Node parent){
	return Node(tag, parent);
}

Node Document::create_text(const char* text, Node parent){
	return Node::text(text, parent);
}

void Document::set_root(Node root){
	root_node = root;
}

Node Document::root() const{
	return root_node;
}

std::string Document::to_string(bool inline_css) const {
	std::stringstream ss;

	traverse([&](TraversalState it){
		Node node = it.node;
		int indent = (it.depth-1) * 2;

		if ( strcmp(node.tag_name(), "") != 0 ){
			ss << std::string(indent, ' ');
			ss << '<';
			if ( it.order == POST_ORDER ){
				ss << '/';
			}
			ss << node.tag_name();
			if ( it.order == PRE_ORDER ){
				for ( auto attr: node.attributes() ){
					if(inline_css && attr.first == "style") continue;
					ss << ' ' << attr.first << '=' << '"' << attr.second << '"';
				}
			}

			if(inline_css) {
				ss << " style='";
				for(const auto& prop : node.css_properties()) {
					ss << prop.first << ": " << prop.second.to_string() << ";";
				}
				ss << "'";
			}

			ss << '>' << std::endl;
		} else {
			if ( it.order == PRE_ORDER ){
				ss << std::string(indent, ' ');
				ss << node.text_content() << std::endl;
			}
		}
	}, BOTH_ORDER);

	return ss.str();
}

Node Document::getElementById(const char* id) const {
	return getElementById(id, root_node);
}

Node Document::getElementById(const char* id, Node root) const {
	Node match;

	traverse([&id,&match](TraversalState it){
		Node node = it.node;
		if ( node.get_attribute("id") == id ){
			match = node;
			it.stop = true;
		}
	}, root, PRE_ORDER);

	return match;
}

static int min(int a, int b){
	return (a<b) ? a : b;
}

std::vector<Node> Document::find(const char* selector) const {
	return find(css::Selector(selector), root_node, 0);
}

std::vector<Node> Document::find(const char* selector, Node node) const {
	return find(css::Selector(selector), node, 0);
}

std::vector<Node> Document::find(const css::Selector& selector) const {
	return find(selector, root_node, 0);
}

std::vector<Node> Document::find(const css::Selector& selector, Node node) const {
	return find(selector, node, 0);
}

std::vector<Node> Document::find(const css::Selector& selector, Node node, size_t begin) const {
	std::vector<Node> matches;
	const auto units = selector.units();
	const size_t num = units.size();
	const size_t next = min(num-1, begin+1);
	const bool last = (begin+1) == num;
	const auto unit = units[begin];

	traverse([&](TraversalState it){
		Node node = it.node;

		assert(node.exists());
		if ( unit.match(node) ){
			// TODO: Check combinator
			if ( !last ){
				const auto sub = find(selector, node, next);
				matches.insert(matches.end(), sub.begin(), sub.end());
				it.skip_subtree = true;
			} else {
				matches.push_back(node);
			}
		}
	}, node, PRE_ORDER);

	return matches;
}

void Document::traverse(std::function<void(TraversalState& it)> callback, TraversalOrder order) const {
	return traverse(callback, root_node, order);
}

void Document::traverse(std::function<void(TraversalState& it)> callback, Node node, TraversalOrder order) const {
	TraversalState state = {
		node, 0, order, false, false
	};
	return traverse(callback, state, order);
}

void Document::traverse(std::function<void(TraversalState& it)> callback, TraversalState& parent_state, TraversalOrder order) const {
	Node current = parent_state.node;
	TraversalState state = {
		current, parent_state.depth + 1, order, false, false
	};

	if ( state.depth >= MAX_DEPTH ){
		return;
	}

	if ( order == PRE_ORDER || order == BOTH_ORDER ){
		state.order = PRE_ORDER;
		callback(state);
		if ( state.stop || state.skip_subtree ){
			return;
		}
	}

	for ( auto it : state.node.children() ){
		state.node = it;
		traverse(callback, state, order);
		if ( state.stop  ){
			parent_state.stop = false;
			return;
		}
	}

	if ( order == POST_ORDER || order == BOTH_ORDER ){
		state.node = current;
		state.order = POST_ORDER;
		callback(state);
		if ( state.stop || state.skip_subtree ){
			return;
		}
	}
}

void Document::apply_css(const css::CSS* css) {
	css->apply_to_document(*this);
}

static void prepare_render(std::function<void(Node node, const State&)> callback, Node node, int depth, const State& parent_state){
	if ( depth >= MAX_DEPTH ){
		return;
	}

	State state;
	state.display = DISPLAY_INLINE;
	state.color.val = 0x000000ff;
	state.background_color.val = 0xffffffff;
	state.width = {0, UNIT_AUTO};
	state.height = {0, UNIT_AUTO};

	apply_css_to_state(node, &state, &parent_state);

	for ( auto it : node.children() ){
		prepare_render(callback, it, depth+1, state);
	}

	callback(node, state);
}

static void prepare_render(std::function<void(Node node, const State&)> callback, Node node){
	State state;
	state.display = DISPLAY_BLOCK;
	state.color.val = 0x000000ff;
	state.background_color.val = 0xffffffff;
	state.width = {0, UNIT_AUTO};
	state.height = {0, UNIT_AUTO};

	prepare_render(callback, node, 0, state);
}

void Document::prepare_render(){
	if ( !root_node.is_invalidated() ){
		return;
	}

	printf("prepare render\n");

	dom::prepare_render([](Node cur, const State& state){
		printf("tag: %s\n", cur.tag_name());
		printf("  id: %s\n", cur.get_attribute("id"));
		printf("  display: %d\n", state.display);
		printf("  color: #%08x\n", state.color.val);
		printf("  background-color: #%08x\n", state.background_color.val);
		printf("  width: %f %d\n", state.width.scalar, state.width.unit);
		printf("  height: %f %d\n", state.height.scalar, state.height.unit);

		cur.clear_invalidated();
	}, root_node);
}

struct css::parsers::property property_table[] = {
	{"background-color",    "transparent",   NO_INHERIT, offsetof(State, background_color),  sizeof(State::background_color),  css::parsers::color},
	{"color",               "#000",             INHERIT, offsetof(State, color),             sizeof(State::color),             css::parsers::color},
	{"display",             "inline",        NO_INHERIT, offsetof(State, display),           sizeof(State::display),           css::parsers::display},
	{"height",              "auto",          NO_INHERIT, offsetof(State, height),            sizeof(State::height),            css::parsers::length},
	{"width",               "auto",          NO_INHERIT, offsetof(State, width),             sizeof(State::width),             css::parsers::length},
	{nullptr, nullptr, NO_INHERIT, 0, 0, nullptr}, /* sentinel */
};

static void apply_property_to_state(Node node, css::parsers::property* property, State* state, const State* parent){
	auto value = node.get_css_property(property->name);
	void* dst = ((char*)state) + property->offset;
	const void* inherit = ((const char*)parent) + property->offset;

	if ( value != nullptr ){
		property->parser(dst, value);
	} else {
		switch ( property->inherit ){
		case INHERIT:
			memcpy(dst, inherit, property->size);
			break;
		case NO_INHERIT:
		{
			/* @todo create initial values only once */
			auto expr = css::Expression::single_term(css::Term::TYPE_STRING, property->initial);
			property->parser(dst, &expr);
			break;
		}
		}
	}
}

static void apply_css_to_state(Node node, State* state, const State* parent) {
	auto cur = property_table;
	while ( cur->name ){
		apply_property_to_state(node, cur, state, parent);
		cur++;
	}
}

}
