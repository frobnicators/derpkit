#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/derpkit.hpp>
#include <derpkit/version.hpp>

#include <derpkit/utils/utils.hpp>
#include <derpkit/dom/document.hpp>
#include <derpkit/dom/node.hpp>
#include <derpkit/css/css.hpp>
#include <derpkit/dom/inspector.hpp>

#include <derpkit/render/rendertarget.hpp>
#include <derpkit/render/shader.hpp>
#include <derpkit/render/utils.hpp>
#include <derpkit/render/draw.hpp>
#include <derpkit/render/texture.hpp>

#include "window.hpp"
#include "render/impl.hpp"
#include "gen/shaderdefs.hpp"
#include "render/text.hpp"

#include <cstdio>
#include <cstdlib>

using namespace derpkit;
using namespace dom;
using namespace render;

int main(int argc, char * argv[]) {
	printf("derpkit library version: %s\n", derpkit::version_string());

	{
		Window window(1280, 720);
		Shader::initialize();

		//if(argc != 2) {
			//printf("Requires exactly one argument: %s css-file\n", argv[0]);
			//exit(1);
		//}

		derpkit::initialize();
		Document doc;

		//css::CSS * css = css::CSS::from_file(argv[1]);
		//css->print();

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

		css::CSS* css = css::CSS::from_source(R"(
		* {
			display: inline;
			color: black;
		}

		div {
			display: block;
		}

		html {
			display: block;
		}

		body {
			display: block;
			background: white;
		}

		#my-div1 {
			width: 800px;
			height: 120in;
		}

		.bar {
			color: red;
			background: 0% 0% green;
		}

		#test-1 {
			color: #00f;
		}

		#test-2 {
			color: #00f;
			height: 25%;
		}

		.baz #test-2 {
			color: #00fa02;
		}

		#my-div3 {
			width: .024cm;
			height: 24.54mm;
			background-color: rgba(200, 50, 24, 125);
		}

		)");

		doc.apply_css(css);

		//css->print();

		//printf("%s\n", doc.to_string(true).c_str());

		Inspector inspector;
		inspector.set_document(&doc);

		RenderTarget rt(1280, 720);
		Texture texture("/data/debug.jpg");

		TextHandle text;
		text.update(box(10,10,201,50), "abcdefiwiwi 012", FontDefinition::create("arial", 24));

		float scale = 0.f;
		float rotation = 0.f;

		while(window.running()) {
			utils::usleep(100);
			inspector.update();

			impl::begin_frame();
			rt.begin_frame();

			Transform trans(vec2(100.f, 100.f), rotation, vec2(sin(scale) + 1.5f));

			rotation += 0.0002f;
			scale += 0.00001f;

			Draw::push_transform(trans);

			Draw::rect(box(0.f, 0.f, 512.f, 512.f), texture);

			if ( text.is_dirty() ){
				Text::draw(text);
			}

			Text::blit(text, vec4(1.f, 0.f, 1.f, 1.f));

			Draw::pop_transform();


			rt.end_frame();
			impl::end_frame();

			impl::clear(vec4(1.f, 0.f, 1.f, 1.f));
			window.blit(&rt);

			window.update();

			Utils::check_for_errors("end_main");
			Shader::unbind();
		}

		Shader::cleanup();
	}

	derpkit::cleanup();
}
