#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/version.hpp>

#include <derpkit/utils/utils.hpp>
#include <derpkit/dom/document.hpp>
#include <derpkit/dom/node.hpp>
#include <derpkit/css/css.hpp>
#include <derpkit/dom/inspector.hpp>

#include <derpkit/render/rendertarget.hpp>
#include <derpkit/render/shader.hpp>
#include <derpkit/render/utils.hpp>
#include <derpkit/render/texture.hpp>

#include "window.hpp"
#include "render/impl.hpp"

#include <cstdio>
#include <cstdlib>

using namespace derpkit;
using namespace dom;
using namespace render;

int main(int argc, char * argv[]) {
	printf("derpkit library version: %s\n", derpkit::version_string());

	{
		Window window(1280, 720);

		//if(argc != 2) {
			//printf("Requires exactly one argument: %s css-file\n", argv[0]);
			//exit(1);
		//}

		Inspector::initialize();
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

		css->print();

		printf("%s\n", doc.to_string(true).c_str());

		Inspector inspector;
		inspector.set_document(&doc);

		RenderTarget rt(1280, 720);
		Shader* shader = Shader::from_file("/data/shaders/main");

		Shader::Uniform u_tex0 = shader->get_uniform("texture0");
		shader->bind();
		u_tex0.set(0);

		Texture texture("/data/debug.jpg");

		while(window.running()) {
			utils::usleep(100);
			inspector.update();

			//rt.begin_frame();
			//rt.end_frame();

			impl::clear(vec4(1.f, 1.f, 1.f, 1.f));
			shader->bind();
			texture.bind();

			Shader::set_projection(window.screenortho());
			Utils::draw_rect(0.f, 0.f, 512.f, 512.f);

			shader->unbind();

			window.update();

			Utils::check_for_errors("end_main");
		}

		delete shader;
	}

	Inspector::cleanup();
}
