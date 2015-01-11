#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/render/utils.hpp>
#include <derpkit/render/shader.hpp>
#include <derpkit/render/math.hpp>

#include "impl.hpp"

namespace derpkit {
namespace render {


void Utils::draw_rect(const vec2& pos, const vec2& size) {
	Shader::set_model_matrix(model_matrix(pos, size));
	impl::draw_rect();
}

void Utils::draw_rect(const box& box) {
	Shader::set_model_matrix(model_matrix(box));
	impl::draw_rect();
}

void Utils::check_for_errors(const char* context) {
	impl::check_for_errors(context);
}


}
}
