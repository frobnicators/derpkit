#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "derpkit/version.hpp"

#include "derpkit/dom/document.hpp"
#include "derpkit/dom/node.hpp"
#include "css/css.hpp"

#include <cstdio>
#include <cstdlib>

using namespace dom;

int main(int argc, char * argv[]) {
	printf("derpkit library version: %s\n", derpkit::version_string());

	if(argc != 2) {
		printf("Requires exactly one argument: %s css-file\n", argv[0]);
		exit(1);
	}

	printf("Parse CSS\n");
	css::CSS * css = css::CSS::from_file(argv[1]);
	css->print();

	printf("Parse selector: \n");

	printf("Build dom\n");
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

	css->apply_to_document(doc);

	printf("%s\n", doc.to_string(true).c_str());
}
