#ifndef DERPKIT_DOM_NODE_HPP
#define DERPKIT_DOM_NODE_HPP

#include <derpkit/utils/string.hpp>
#include <derpkit/css/specificity.hpp>
#include <derpkit/css/expression.hpp>
#include <vector>
#include <map>
#include <memory>

namespace css {
	class Rule;
};

namespace dom {

struct NodeCSSProperty {
	const std::vector<css::Expression>* expressions;
	css::Specificity specificity;

	NodeCSSProperty(const std::vector<css::Expression>* expressions, const css::Specificity& specificity)
		: expressions(expressions), specificity(specificity)
	{ }

	void operator=(const NodeCSSProperty& o) {
		expressions = o.expressions;
		specificity = specificity;
	}

	std::string to_string() const;
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

	size_t children_count() const;
	std::vector<Node> children() const;
	const char* tag_name() const;
	const char* text_content() const;

	const std::vector<std::string>& classes() const;

	std::map<std::string,NodeCSSProperty>& css_properties();
	const css::Expression* get_css_property(const char* property, unsigned int index = 0) const;
	const css::Expression* get_css_property(const char* property, unsigned int index, unsigned int &count) const;

	/**
	 * Map from rule to list of matched selector indices
	 */
	std::map<const css::Rule*, std::vector<int>>& matched_css_rules();

	void attach(Node);
	void detach();

	void invalidate();
	void clear_invalidated();
	bool is_invalidated() const;

	uint64_t get_internal_id() const;

	static Node from_internal_id(uint64_t);

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
