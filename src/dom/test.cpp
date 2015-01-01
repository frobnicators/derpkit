#include "dom/document.hpp"
#include "dom/node.hpp"
#include <cstdio>
#include <string>

using namespace dom;

int main(){
	Document doc;

	Node root = doc.create_element("html");
	Node body = doc.create_element("body", root);
	body.set_attribute("id", "body-element");

	Node div1 = doc.create_element("div", body);
	div1.set_attribute("id", "my-div1");
	div1.set_attribute("class", "bar baz");

	Node div2 = doc.create_element("div");
	div2.attach(div1);
	div2.detach();

	doc.set_root(root);
	doc.visit_depthfirst([](Node cur){
		printf("tag: %s\n", cur.tag_name());
		printf("id: %s\n", cur.get_attribute("id"));
	});
}
