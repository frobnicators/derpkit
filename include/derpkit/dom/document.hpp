#ifndef DERPKIT_DOM_DOCUMENT_HPP
#define DERPKIT_DOM_DOCUMENT_HPP

#include <functional>
#include <derpkit/dom/node.hpp>
#include <derpkit/css/selector.hpp>

namespace dom {

enum TraversalOrder {
	/*      A
	 *    B   C
	 */
	POST_ORDER,            /* visits current node after children:    B, C, A */
	PRE_ORDER,             /* visits current node before children:   A, B, C */
	BOTH_ORDER,            /* visits current node both pre and post: A, B, B, C, C, A */
};

struct TraversalState {
	/* input */
	Node node;
	int depth;
	TraversalOrder order;  /* current order (most useful when using BOTH_ORDER in which case it is set to PRE or POST) */

	/* output */
	bool stop;             /* if set to true, traversal stop entierly */
	bool skip_subtree;     /* if set to true, traversal skips the rest of the children but keeps traversing */
};

class DERPKIT_EXPORT Document {
public:
	Document();

	Node create_element(const char* tag);
	Node create_element(const char* tag, Node parent);
	Node create_text(const char* text, Node parent);
	void set_root(Node root);
	Node root() const;
	std::string to_string(bool inline_css=false) const;

	Node getElementById(const char* id) const;
	Node getElementById(const char* id, Node node) const;

	/**
	 * Apply CSS to tree
	 */
	void apply_css(const css::CSS* css);

	/**
	 * Find nodes from CSS selector.
	 */
	std::vector<Node> find(const char* selector) const;
	std::vector<Node> find(const char* selector, Node node) const;
	std::vector<Node> find(const css::Selector& selector) const;
	std::vector<Node> find(const css::Selector& selector, Node node) const;

	/**
	 * Visits all nodes, depth first
	 */
	void traverse(std::function<void(TraversalState& it)>, TraversalOrder order) const;
	void traverse(std::function<void(TraversalState& it)>, Node node, TraversalOrder order) const;

	void prepare_render();

private:
	std::vector<Node> find(const css::Selector& selector, Node node, size_t state) const;

	void traverse(std::function<void(TraversalState& it)>, TraversalState& state, TraversalOrder order) const;

	Node root_node;
};

}

#endif /* DERPKIT_DOM_DOCUMENT_HPP */
