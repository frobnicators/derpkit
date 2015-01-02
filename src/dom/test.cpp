#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "css/css.hpp"
#include "dom/document.hpp"
#include "dom/node.hpp"
#include <cstdio>
#include <string>

using namespace dom;

int main(){
	Document doc;

	css::CSS * css = css::CSS::from_source(
"#my-div1 b {\n"
"	display: inline;\n"
"}\n"
		);

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

	doc.create_element("b", div1).set_attribute("id", "test-1");
	doc.create_element("b", div1).set_attribute("id", "test-2");
	doc.create_element("b", div3).set_attribute("id", "test-3");

	doc.set_root(root);
	printf("%s\n", doc.to_string().c_str());;

	for ( auto rule : css->rules() ){
		for ( auto selector : rule.selectors() ){
			selector.print();
			putc('\n',stdout);
			for ( auto match : doc.find(selector) ){
				printf("match: %s / %s\n", match.tag_name(), match.get_attribute("id"));
			}
		}
	}

	return 0;
	doc.prepare_render([](Node cur, const State& state){
		printf("tag: %s\n", cur.tag_name());
		printf("  id: %s\n", cur.get_attribute("id"));
		printf("  display: %d\n", state.display);
		printf("  color: %d\n", state.color.val);
		printf("  background-color: %d\n", state.background_color.val);
		printf("  width: %f %d\n", state.width.scalar, state.width.unit);
		printf("  height: %f %d\n", state.height.scalar, state.height.unit);
	});
}
