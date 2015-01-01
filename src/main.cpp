#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "derpkit/version.hpp"

#include "dom/document.hpp"
#include "dom/node.hpp"
#include "css/css.hpp"

#include <cstdio>
#include <cstdlib>

int main(int argc, char * argv[]) {
	printf("derpkit library version: %s\n", derpkit::version_string());

	if(argc != 2) {
		printf("Requires exactly one argument: %s css-file\n", argv[0]);
		exit(1);
	}

	printf("Parse CSS\n");
	css::CSS * css = css::CSS::from_file(argv[1]);
	css->print();

	printf("Build dom\n");
	dom::Node root("html");
	dom::Node body("body", &root);

	dom::Node div1("div", &body);
	div1.set_attribute("id", "foo");
	div1.set_attribute("class", "bar baz");

	dom::Node div2("div");
	div2.attach(&div1);
	div2.detach();
}
