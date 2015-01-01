#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "node.hpp"
#include <derpkit/utils/string_utils.hpp>
#include <cstring>
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
	std::vector<std::string> classes;
	std::map<std::string,NodeCSSProperty> css_properties;

	NodeImpl(const char* tag)
		: tag(tag)
		, parent(){

	}

	const char* get_attribute(const char* key) const {
		auto it = attribute.find(key);
		if ( it != attribute.end() ){
			return it->second.c_str();
		} else {
			return nullptr;
		}
	}

	bool has_attribute(const char* key) const {
		auto it = attribute.find(key);
		return it != attribute.end();
	}

	void set_attribute(const char* key, const char* value) {
		attribute[key] = value;

		if ( strcmp(key, "class") == 0 ){
			parse_classes();
		}
	}

	void parse_classes(){
		classes = str_split(get_attribute("class"), " ", SPLIT_TRIM | SPLIT_IGNORE_EMPTY);
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
	return _impl->get_attribute(key);
}

bool Node::has_attribute(const char* key) const {
	return _impl->has_attribute(key);
}

void Node::set_attribute(const char* key, const char* value) {
	_impl->set_attribute(key, value);
}

const std::vector<std::string>& Node::classes() const {
	return _impl->classes;
}

std::map<std::string,NodeCSSProperty>& Node::css_properties() {
	return _impl->css_properties;
}

const char* Node::get_css_property(const char* property) const {
	auto it=_impl->css_properties.find(property);
	if(it == _impl->css_properties.end()) {
		return nullptr;
	}

	return it->second.property.c_str();
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
