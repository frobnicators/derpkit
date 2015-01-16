#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/render/draw.hpp>
#include <derpkit/render/shader.hpp>
#include <derpkit/render/math.hpp>

#include "impl.hpp"

namespace derpkit {
namespace render {

void Draw::rect(const box& box) {
	Shader::set_model_matrix(model_matrix(box));
	impl::draw_rect();
}

void Draw::rect(const box& box, const vec4& color, ShaderId shader) {
	const Shader* shader_ptr = Shader::get(shader);
	shader_ptr->bind();
	shader_ptr->get_uniform(Uniform_color).set(color);
	Draw::rect(box);
}

void Draw::rect(const box& box, const Texture& texture, const vec4& color, ShaderId shader) {
	texture.bind();
	Draw::rect(box, color, shader);
}

}
}
