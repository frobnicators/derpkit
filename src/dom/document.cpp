#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "document.hpp"

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

void Document::visit_depthfirst(std::function<void(Node node, const State&)> callback){
	State state;
	state.display = DISPLAY_BLOCK;
	state.color.val = 0x000000ff;
	state.background_color.val = 0xffffffff;
	state.width = {0, UNIT_AUTO};
	state.height = {0, UNIT_AUTO};

	visit_depthfirst(callback, root_node, 0, state);
}

void Document::visit_depthfirst(std::function<void(Node node, const State&)> callback, Node node, int depth, const State& parent_state){
	if ( depth >= MAX_DEPTH ){
		return;
	}

	State state = parent_state;

	for ( auto it : node.children() ){
		visit_depthfirst(callback, it, depth+1, state);
	}

	callback(node, state);
}

}
