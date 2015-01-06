#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/dom/node.hpp>
#include <derpkit/utils/string_utils.hpp>
#include <cstring>
#include <string>
#include <sstream>
#include <map>
#include <cassert>

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
	bool invalid;
	std::string text;

	NodeImpl(const char* tag)
		: tag(tag)
		, parent()
		, invalid(true) {

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
		auto ptr = self.lock();
		assert(!self.expired());
		parent->children.push_back(ptr);
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

	void invalidate(){
		invalid = true;
		auto ptr = parent.lock();
		if ( ptr ){
			ptr->invalidate();
		}
	}
};

Node::Node(){

}

Node::Node(const Node& rhs){
	_impl = rhs._impl;
}

Node::Node(std::shared_ptr<NodeImpl> node){
	_impl = node;
}

Node::Node(const char* tag){
	auto ptr = std::make_shared<NodeImpl>(tag);
	ptr->self = ptr;
	_impl = ptr;
}

Node::Node(const char* tag, Node parent){
	auto ptr = std::make_shared<NodeImpl>(tag);
	ptr->self = ptr;
	_impl = ptr;

	attach(parent);
}

Node Node::text(const char* text, Node parent){
	Node node = Node("", parent);
	node._impl->text = text;
	return node;
}


bool Node::exists() const {
	return _impl.get();
}

bool Node::is_textnode() const {
	return strcmp(tag_name(), "") == 0;
}

const std::map<std::string, std::string>& Node::attributes() const {
	return _impl->attribute;
}

const char* Node::get_attribute(const char* key) const {
	assert(_impl.get());
	return _impl->get_attribute(key);
}

bool Node::has_attribute(const char* key) const {
	assert(_impl.get());
	return _impl->has_attribute(key);
}

void Node::set_attribute(const char* key, const char* value) {
	assert(_impl.get());
	_impl->set_attribute(key, value);
}

const char* Node::tag_name() const {
	assert(_impl.get());
	return _impl->tag.c_str();
}

const char* Node::text_content() const {
	assert(_impl.get());
	return _impl->text.c_str();
}

size_t Node::children_count() const {
	assert(_impl.get());
	return _impl->children.size();
}

std::vector<Node> Node::children() const {
	assert(_impl.get());
	std::vector<Node> children;
	for ( auto it : _impl->children ){
		children.push_back(Node(it));
	}
	return children;
}

const std::vector<std::string>& Node::classes() const {
	return _impl->classes;
}

std::map<std::string,NodeCSSProperty>& Node::css_properties() {
	return _impl->css_properties;
}

const css::Expression* Node::get_css_property(const char* property, unsigned int index) const {
	unsigned int count;
	return get_css_property(property, index, count);
}

const css::Expression* Node::get_css_property(const char* property, unsigned int index, unsigned int &count) const {
	auto it=_impl->css_properties.find(property);
	if(it == _impl->css_properties.end()) {
		return nullptr;
	}

	count = it->second.expressions->size();

	if(it->second.expressions->size() > index)
		return &((*it->second.expressions)[index]);
	else
		return nullptr;
}

void Node::attach(Node parent){
	assert(_impl.get());

	/* detatch from previous node first */
	_impl->detach();

	_impl->attach(parent._impl);
}

void Node::detach(){
	assert(_impl.get());
	_impl->detach();
}

void Node::invalidate(){
	assert(_impl.get());
	_impl->invalidate();
}

void Node::clear_invalidated(){
	assert(_impl.get());
	_impl->invalid = false;
}

bool Node::is_invalidated() const {
	assert(_impl.get());
	return _impl->invalid;
}

uint64_t Node::get_internal_id() const {
	assert(_impl.get());
	return (uint64_t)_impl.get();
}

Node Node::from_internal_id(uint64_t id) {
	NodeImpl* impl = (NodeImpl*)id;
	return Node(impl->self.lock());
}

std::string NodeCSSProperty::to_string() const {
	std::stringstream ss;
	auto it = expressions->begin();
	if(it != expressions->end()) {
		ss << (it++)->to_string();
	}
	while ( it != expressions->end()) {
		ss << ", ";
		ss << (it++)->to_string();
	}

	return ss.str();
}

}
