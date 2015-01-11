#ifndef DERPKIT_RENDER_SHADERRESOURCE_HPP
#define DERPKIT_RENDER_SHADERRESOURCE_HPP

#include <string>
#include <cstring>
#include "render/impl.hpp"

namespace derpkit {
namespace render {

struct SamplerDef {
	const char* uniform_name;
	int texture;
	// TODO: Maybe some more sampler data
	bool operator<(const SamplerDef& o) const { return strcmp(uniform_name, o.uniform_name) < 0; }
};

struct ShaderSourceDef {
	const char* name;
	const char* source;
	impl::ShaderStageId stage;
	const SamplerDef* samplers;
	const int* uniforms;
};

struct ShaderProgramDef {
	const char* name;
	const unsigned int stages[2];
};

}
}

#endif
