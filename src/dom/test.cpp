#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/css/css.hpp>
#include <derpkit/dom/document.hpp>
#include <derpkit/dom/node.hpp>
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

	Node div3 = doc.create_element("div", body);
	div3.set_attribute("id", "my-div3");

	Node b1 = doc.create_element("b", div1);
	b1.set_attribute("id", "test-1");
	doc.create_text("foobar", b1);

	doc.create_text("lorem ipsum", div1);

	doc.create_element("b", div1).set_attribute("id", "test-2");
	doc.create_element("b", div3).set_attribute("id", "test-3");

	doc.set_root(root);
	printf("%s\n", doc.to_string().c_str());

	for ( auto match : doc.find("#my-div1 b") ){
		printf("match: %s#%s\n", match.tag_name(), match.get_attribute("id"));
	}

	doc.prepare_render();
	doc.prepare_render();
	doc.prepare_render();

	return 0;
}
