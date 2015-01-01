#include "dom/document.hpp"
#include "dom/node.hpp"
#include <cstdio>
#include <string>

using namespace dom;

int main(){
	Node root("html");
	Node body("body", &root);

	Node div1("div", &body);
	div1.set_attribute("id", "foo");
	div1.set_attribute("class", "bar baz");

	printf("id: %s\n", div1.get_attribute("id"));
	for ( std::string it: div1.classes() ){
		printf("%s\n", it.c_str());
	}

	Node div2("div");
	div2.attach(&div1);
	div2.detach();
}
