#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "document.hpp"
#include <cassert>
#include <sstream>

static const int MAX_DEPTH = 200;

namespace dom {

Document::Document(){

}

Node Document::create_element(const char* tag){
	return Node(tag);
}


Node Document::create_element(const char* tag, Node parent){
	return Node(tag, parent);
}

void Document::set_root(Node root){
	root_node = root;
}

Node Document::root() const{
	return root_node;
}

std::string Document::to_string() const {
	std::stringstream ss;

	traverse([&](TraversalState it){
		Node node = it.node;
		int indent = (it.depth-1) * 2;
		ss << std::string(indent, ' ');
		ss << '<';
		if ( it.order == POST_ORDER ){
			ss << '/';
		}
		ss << node.tag_name();
		if ( it.order == PRE_ORDER ){
			for ( auto attr: node.attributes() ){
				ss << ' ' << attr.first << '=' << '"' << attr.second << '"';
			}
		}
		ss << '>' << std::endl;
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

std::vector<Node> Document::find(const css::Selector& selector, Node node, int begin) const {
	std::vector<Node> matches;
	const auto atoms = selector.atoms();
	const auto num = atoms.size();
	const int next = min(num-1, begin+1);
	const bool last = (begin+1) == num;
	const auto atom = atoms[begin];

	traverse([&](TraversalState it){
		Node node = it.node;

		assert(node.exists());
		if ( atom.match(node) ){
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

void Document::prepare_render(std::function<void(Node node, const State&)> callback){
	State state;
	state.display = DISPLAY_BLOCK;
	state.color.val = 0x000000ff;
	state.background_color.val = 0xffffffff;
	state.width = {0, UNIT_AUTO};
	state.height = {0, UNIT_AUTO};

	prepare_render(callback, root_node, 0, state);
}

void Document::prepare_render(std::function<void(Node node, const State&)> callback, Node node, int depth, const State& parent_state){
	if ( depth >= MAX_DEPTH ){
		return;
	}

	State state = parent_state;

	for ( auto it : node.children() ){
		prepare_render(callback, it, depth+1, state);
	}

	callback(node, state);
}

}
