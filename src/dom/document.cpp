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

void Document::visit_depthfirst(std::function<void(Node node)> callback){
	visit_depthfirst(callback, root_node, 0);
}

void Document::visit_depthfirst(std::function<void(Node node)> callback, Node node, int depth){
	if ( depth >= MAX_DEPTH ){
		return;
	}

	for ( auto it : node.children() ){
		visit_depthfirst(callback, it, depth+1);
	}

	callback(node);
}

}
