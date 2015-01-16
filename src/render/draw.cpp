#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/utils/logging.hpp>
#include <derpkit/render/draw.hpp>
#include <derpkit/render/shader.hpp>
#include <derpkit/render/math.hpp>

#include "impl.hpp"

namespace derpkit {
namespace render {

#define MATRIX_STACK_SIZE 128

static mat3 g_matrix_stack[MATRIX_STACK_SIZE];
static int g_stack_top = -1;

mat3 Draw::s_transform = mat3();

void Draw::rect(const box& box) {
	Shader::set_model_matrix(s_transform * model_matrix(box));
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

void Draw::push_transform(const Transform& transform) {
	if(g_stack_top == MATRIX_STACK_SIZE) {
		Logging::fatal("Matrix Stack full! (Matrix Size: %d\n", MATRIX_STACK_SIZE);
	}

	// If we allow modifying transform in any other way than
	// this function we must save the initial matrix too.
	// At the moment we asume it's identity
	if(g_stack_top > -1)
	{
		g_matrix_stack[g_stack_top] = s_transform;
	}
	s_transform = s_transform * transform.matrix();
	++g_stack_top;

}

void Draw::pop_transform() {
	if(g_stack_top > 0) {
		s_transform = g_matrix_stack[g_stack_top];
	} else if(g_stack_top == 0) {
		s_transform = mat3();
	} else {
		Logging::fatal("Matrix push/pop missmatch (too many pops)\n");
	}
	--g_stack_top;
}

}
}
