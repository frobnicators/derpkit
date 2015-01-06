#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/dom/document.hpp>
#include <derpkit/css/css.hpp>
#include <derpkit/css/state.hpp>
#include "css/parsers.hpp"
#include <cassert>
#include <cstddef>
#include <sstream>

static const int MAX_DEPTH = 200;

using css::parsers::INHERIT;
using css::parsers::NO_INHERIT;

namespace dom {

static void apply_css_to_state(Node node, css::State* state, const css::State* parent);

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

	traverse(BOTH_ORDER, [&](TraversalState it){
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
				if(inline_css) {
					ss << " style=\"";
					for(const auto& prop : node.css_properties()) {
						ss << prop.first << ": " << prop.second.to_string() << ";";
					}
					ss << '"';
				}
			}

			ss << '>' << std::endl;
		} else {
			if ( it.order == PRE_ORDER ){
				ss << std::string(indent, ' ');
				ss << node.text_content() << std::endl;
			}
		}
	});

	return ss.str();
}

Node Document::getElementById(const char* id) const {
	return getElementById(id, root_node);
}

Node Document::getElementById(const char* id, Node root) const {
	Node match;

	traverse(root, PRE_ORDER, [&id,&match](TraversalState it){
		Node node = it.node;
		if ( node.get_attribute("id") == id ){
			match = node;
			it.stop = true;
		}
	});

	return match;
}

static int min(int a, int b){
	return (a<b) ? a : b;
}

static int max(int a, int b){
	return (a>b) ? a : b;
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

	traverse(node, PRE_ORDER, [&](TraversalState it){
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
	});

	return matches;
}

void Document::traverse(TraversalOrder order, std::function<void(TraversalState& it)> callback) const {
	return traverse(root_node, order, callback);
}

void Document::traverse(Node node, TraversalOrder order, std::function<void(TraversalState& it)> callback) const {
	TraversalState state = {
		node, 0, order, false, false
	};
	return traverse(state, order, callback);
}

void Document::traverse(TraversalState& parent_state, TraversalOrder order, std::function<void(TraversalState& it)> callback) const {
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
		traverse(state, order, callback);
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

static void compute_state(Node node, int depth, const css::State* parent_state){
	if ( depth >= MAX_DEPTH ){
		return;
	}

	apply_css_to_state(node, node.computed_state(), parent_state);

	for ( auto it : node.children() ){
		compute_state(it, depth+1, node.computed_state());
	}

	node.clear_invalidated();
}

static void compute_state(Node node, int root_width, int root_height){
	css::State state;
	state.display = css::DISPLAY_BLOCK;
	state.color.val = 0x000000ff;
	state.background_color.val = 0xffffffff;
	state.width = {(float)root_width, css::UNIT_PX};
	state.height = {(float)root_height, css::UNIT_PX};
	compute_state(node, 0, &state);
}

static float estimate_text_width(const char* text){
	/* hack until font-rendering is in place */
	const size_t num_chars = strlen(text);
	return num_chars * 15;
}

static float estimate_text_height(){
	return 15;
}

static void expand_auto_size(css::Length& dst, const css::Length& src){
	if ( dst.unit != css::UNIT_AUTO ) return;
	if ( src.unit != css::UNIT_PX ) return;

	dst.unit = css::UNIT_PX;
	dst.scalar = src.scalar; /* @todo padding */
}

static void expand_inline(Node node){
	auto state = node.computed_state();

	state->width.unit  = css::UNIT_PX;
	state->height.unit = css::UNIT_PX;

	if ( node.is_textnode() ){
		state->width.scalar  = estimate_text_width(node.text_content());
		state->height.scalar = estimate_text_height();
	} else {
		/* On this pass no breaks is inserted, just keep adding those inlines
		 * together. Breaks is resolved later when container size is known */
		state->width.scalar = 0;
		state->height.scalar = 0;
		for ( auto child: node.children() ){
			state->width.scalar += child.computed_state()->width.scalar;
			state->height.scalar = max(state->height.scalar, child.computed_state()->height.scalar);
		}
	}
}

void Document::prepare_render(int width, int height){
	if ( !root_node.is_invalidated() ){
		return;
	}

	printf("prepare render\n");

	/* compute initial style */
	dom::compute_state(root_node, width, height);

	/* derpkit: size of root node is always the window size */
	{
		auto state = root_node.computed_state();
		state->width.scalar  = width;
		state->height.scalar = height;
		state->width.unit  = css::UNIT_PX;
		state->height.unit = css::UNIT_PX;
	}

	/* preliminary sizes for nodes which expands themselves (e.g. inline text)*/
	traverse(POST_ORDER, [](TraversalState it){
		auto node = it.node;
		auto state = node.computed_state();

		switch ( state->display ){
		case css::DISPLAY_INLINE:
			expand_inline(node);
			break;

		case css::DISPLAY_NONE:
		case css::DISPLAY_BLOCK:
			break;
		}
	});

	/* starting from root, resolve relative sizes and shrink children to fit (e.g. by inserting breaks on inline) */
	traverse(PRE_ORDER, [this](TraversalState it){
		auto node = it.node;
		if ( node == root_node ) return;

		auto state = node.computed_state();
		auto parent = node.parent().computed_state();

		switch ( state->display ){
		case css::DISPLAY_BLOCK:
			expand_auto_size(state->width,  parent->width);
			expand_auto_size(state->height, parent->height);
			break;

		case css::DISPLAY_NONE:
		case css::DISPLAY_INLINE:
			break;
		}
	});

	/* final pass: debug print */
	traverse(PRE_ORDER, [](TraversalState it){
		auto node = it.node;
		auto state = node.computed_state();
		printf("tag: %s\n", node.tag_name());
		printf("  id: %s\n", node.get_attribute("id"));
		printf("  display: %s\n", display_to_string(state->display).c_str());
		printf("  position: %s\n", state->position.to_string().c_str());
		printf("  color: #%08x\n", state->color.val);
		printf("  background-color: #%08x\n", state->background_color.val);
		printf("  width: %s\n", length_to_string(state->width).c_str());
		printf("  height: %s\n", length_to_string(state->height).c_str());
	});
}

struct css::parsers::property property_table[] = {
	{"background-color",    "transparent",   NO_INHERIT, offsetof(css::State, background_color),  sizeof(css::State::background_color),  css::parsers::color},
	{"color",               "#000",             INHERIT, offsetof(css::State, color),             sizeof(css::State::color),             css::parsers::color},
	{"display",             "inline",        NO_INHERIT, offsetof(css::State, display),           sizeof(css::State::display),           css::parsers::display},
	{"height",              "auto",          NO_INHERIT, offsetof(css::State, height),            sizeof(css::State::height),            css::parsers::length},
	{"position",            "static",        NO_INHERIT, offsetof(css::State, position),          sizeof(css::State::position),          css::Position::parse},
	{"width",               "auto",          NO_INHERIT, offsetof(css::State, width),             sizeof(css::State::width),             css::parsers::length},
	{nullptr, nullptr, NO_INHERIT, 0, 0, nullptr}, /* sentinel */
};

static void apply_property_to_state(Node node, css::parsers::property* property, css::State* state, const css::State* parent){
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

static void apply_css_to_state(Node node, css::State* state, const css::State* parent) {
	auto cur = property_table;
	while ( cur->name ){
		apply_property_to_state(node, cur, state, parent);
		cur++;
	}
}

}
