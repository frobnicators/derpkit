#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "node.hpp"
#include <string>
#include <map>

namespace dom {

class NodeImpl {
public:
	utils::String tag;
	std::weak_ptr<NodeImpl> self;
	std::weak_ptr<NodeImpl> parent;
	mutable std::vector<std::shared_ptr<NodeImpl>> children;
	std::map<std::string, std::string> attribute;

	NodeImpl(const char* tag)
		: tag(tag)
		, parent(){

	}

	void attach(std::shared_ptr<NodeImpl> parent){
		this->parent = parent;
		parent->children.push_back(self.lock());
	}

	void detach(){
		if ( parent.expired() ) return;

		/* find child element */
		auto ptr = parent.lock();
		auto it = ptr->children.begin();
		for ( ; it < ptr->children.end(); ++it ){
			if ((*it).get() == this ) break;
		}

		if ( it != ptr->children.end() ){
			ptr->children.erase(it);
		}
	}
};

Node::Node(const char* tag){
	auto ptr= std::make_shared<NodeImpl>(tag);
	ptr->self = ptr;
	_impl = ptr;
}

Node::Node(const char* tag, const Node* parent)
	: _impl(std::make_shared<NodeImpl>(tag)){

	attach(parent);
}

const char* Node::get_attribute(const char* key) const {
	auto it = _impl->attribute.find(key);
	if ( it != _impl->attribute.end() ){
		return it->second.c_str();
	} else {
		return nullptr;
	}
}

bool Node::has_attribute(const char* key) const {
	auto it = _impl->attribute.find(key);
	return it != _impl->attribute.end();
}

void Node::set_attribute(const char* key, const char* value) {
	_impl->attribute[key] = value;
}

void Node::attach(const Node* parent){
	if ( !parent) return;

	/* detatch from previous node first */
	_impl->detach();

	_impl->attach(parent->_impl);
}

void Node::detach(){
	_impl->detach();
}

}
