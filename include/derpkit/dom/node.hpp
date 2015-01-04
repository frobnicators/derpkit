#ifndef DERPKIT_DOM_NODE_HPP
#define DERPKIT_DOM_NODE_HPP

#include <derpkit/utils/string.hpp>
#include <derpkit/css/specificity.hpp>
#include <derpkit/export.hpp>
#include <vector>
#include <map>
#include <memory>

namespace dom {

struct NodeCSSProperty {
	std::string value;
	css::Specificity specificity;

	NodeCSSProperty(const std::string& value, const css::Specificity& specificity)
		: value(value), specificity(specificity)
	{ }
};

class NodeImpl;
class DERPKIT_EXPORT Node {
public:
	Node(const Node& rhs);

	bool exists() const;

	const char* get_attribute(const char* key) const;
	bool has_attribute(const char* key) const;
	void set_attribute(const char* key, const char* value);
	const std::map<std::string, std::string>& attributes() const;

	std::vector<Node> children() const;
	const char* tag_name() const;
	const char* text_content() const;

	const std::vector<std::string>& classes() const;

	std::map<std::string,NodeCSSProperty>& css_properties();
	const char* get_css_property(const char* property) const;

	void attach(Node);
	void detach();

	void invalidate();
	void clear_invalidated();
	bool is_invalidated() const;

private:
	friend class Document;

	Node();
	Node(std::shared_ptr<NodeImpl>);
	Node(const char* tag);
	Node(const char* tag, Node parent);

	static Node text(const char* text, Node parent);

	std::shared_ptr<NodeImpl> _impl;
};

}

#endif /* DERPKIT_DOM_NODE_HPP */
