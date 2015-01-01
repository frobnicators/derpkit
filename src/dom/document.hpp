#ifndef DERPKIT_DOM_DOCUMENT_HPP
#define DERPKIT_DOM_DOCUMENT_HPP

#include "state.hpp"
#include <functional>
#include "node.hpp"

namespace dom {

class DERPKIT_EXPORT Document {
public:
	Document();

	Node create_element(const char* tag);
	Node create_element(const char* tag, Node parent);
	void set_root(Node root);
	Node root() const;

	void visit_depthfirst(std::function<void(Node node, const State&)>);

private:
	void visit_depthfirst(std::function<void(Node node, const State&)>, Node node, int depth, const State& state);
	Node root_node;
};

}

#endif /* DERPKIT_DOM_DOCUMENT_HPP */
