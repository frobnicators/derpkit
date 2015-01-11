#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "render/impl.hpp"
#include "gl.hpp"

#include <derpkit/utils/logging.hpp>
#include <derpkit/utils/file.hpp>

#include <vector>
#include <sstream>
#include <algorithm>

namespace derpkit {
namespace render {
namespace impl {

struct Uniform {
	GLint location;
#ifdef ENABLE_DEBUG
	GLuint program;
	std::string name;
#endif
};

struct ShaderStage {
	GLuint resource;

	ShaderStage(GLuint i) : resource(i) {}
};

struct Shader {
	GLuint resource;
};

#ifdef ENABLE_DEBUG
static GLuint bound_shader = 0;
#endif

GLuint compile_shader(GLenum shaderType, const char* name, const char* source) {
	const GLuint shader = glCreateShader(shaderType);

	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	GLint compile_status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

	if ( compile_status == GL_FALSE ) {
		char buffer[2048];

		Logging::error("Failed to compile shader %s: \n", name);
		std::stringstream code(source);
		int linenr=0;
		while(!code.eof()) {
			code.getline(buffer, 2048);
			Logging::error("%4d %s\n", ++linenr, buffer);
		}
		glGetShaderInfoLog(shader, 2048, NULL, buffer);
		Logging::error("Error in shader %s: %s\n", name, buffer);
		return 0;
	}

	return shader;
}

GLuint link_program(const char* shader_name, const std::vector<ShaderStage*> &shaderList) {
	GLint gl_tmp;
	GLuint program = glCreateProgram();

	for(ShaderStage* shader : shaderList) {
		glAttachShader(program, shader->resource);
	}

	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &gl_tmp);

	if(!gl_tmp) {
		char buffer[2048];
		glGetProgramInfoLog(program, 2048, NULL, buffer);
		Logging::error("Link error in shader %s: %s\n", shader_name, buffer);
		return 0;
	}

#ifdef ENABLE_DEBUG
	glValidateProgram(program);

	glGetProgramiv(program, GL_VALIDATE_STATUS, &gl_tmp);

	if(!gl_tmp) {
		char buffer[2048];
		glGetProgramInfoLog(program, 2048, NULL, buffer);
		Logging::error("Validate error in shader %s: %s\n", shader_name, buffer);
		return 0;
	}

#endif

	return program;
}

void free_shader(Shader* shader) {
	if(shader == nullptr) return;
	glDeleteProgram(shader->resource);
	delete shader;
}

void bind_shader(Shader* shader) {
	glUseProgram(shader->resource);
#ifdef ENABLE_DEBUG
	bound_shader = shader->resource;
#endif
}
void unbind_shader() {
	glUseProgram(0);
#ifdef ENABLE_DEBUG
	bound_shader = 0;
#endif
}

static void init_shader(Shader* shader) {
	glBindAttribLocation(shader->resource, 0, "in_pos");
	glBindAttribLocation(shader->resource, 1, "in_uv");
}

ShaderStage* create_shaderstage_from_source(ShaderStageId stageid, const char* name, const char* source) {
	GLenum shader_type;
	switch(stageid) {
		case ShaderStage_Vertex:
			shader_type = GL_VERTEX_SHADER;
			break;
		case ShaderStage_Fragment:
			shader_type = GL_FRAGMENT_SHADER;
			break;
		default:
			Logging::fatal("%s: Invalid shader stage %d\n", name, stageid);
			return nullptr;
	}

	GLuint resource = compile_shader(shader_type, name, source);
	if(resource == 0) return nullptr;

	ShaderStage* stage = new ShaderStage(resource);

	return stage;
}

void free_shaderstage(ShaderStage* stage) {
	if(stage == nullptr) return;
	glDeleteShader(stage->resource);
	delete stage;
}

Shader* create_shader(const char* name, const std::vector<ShaderStage*>& stages) {
	GLuint resource = link_program(name, stages);

	if(resource == 0) return nullptr;

	Shader* shader = new Shader();
	shader->resource = resource;

	init_shader(shader);

	return shader;
}


#ifdef ENABLE_DEBUG
Shader* create_shader_from_filename(const std::string& filename) {
	std::vector<ShaderStage*> shader_list;
	{
		FileData file = load_file(filename + ".vert");
		shader_list.push_back(new ShaderStage(compile_shader(GL_VERTEX_SHADER, filename.c_str(), file.data())));
	}
	{
		FileData file = load_file(filename + ".frag");
		shader_list.push_back(new ShaderStage(compile_shader(GL_FRAGMENT_SHADER, filename.c_str(), file.data())));
	}

	for(ShaderStage* s : shader_list) {
		if(s->resource == 0) {
			std::for_each(shader_list.begin(), shader_list.end(), free_shaderstage);
			return nullptr;
		}
	}

	GLuint resource = link_program(filename.c_str(), shader_list);
	std::for_each(shader_list.begin(), shader_list.end(), free_shaderstage);

	if(resource == 0) return nullptr;

	Shader* shader = new Shader();
	shader->resource = resource;

	init_shader(shader);

	return shader;
}
#endif

Uniform* get_uniform(Shader* shader, const std::string& name) {
	Uniform* uniform = new Uniform();
	uniform->location = glGetUniformLocation(shader->resource, name.c_str());
#ifdef ENABLE_DEBUG
	uniform->program = shader->resource;
	uniform->name = name;
#endif
	return uniform;
}

Uniform* copy_uniform(const Uniform* other) {
	Uniform* uniform = new Uniform();
	uniform->location = other->location;
#ifdef ENABLE_DEBUG
	uniform->program = other->program;
	uniform->name = other->name;
#endif

	return uniform;
}

void free_uniform(Uniform* uniform) {
	delete uniform;
}

static void check_uniform(Uniform* uniform) {
#ifdef ENABLE_DEBUG
	if(bound_shader != uniform->program) Logging::fatal("Wrong shader bound for uniform %s. Required: %u, Bound: %d.\n", uniform->name.c_str(), uniform->program, bound_shader);
#endif
}

void uniform_set(Uniform* uniform, const mat3 &m) {
	check_uniform(uniform);
	glUniformMatrix3fv(uniform->location, 1, 0, value_ptr(m));
}

void uniform_set(Uniform* uniform, const ivec2 &v) {
	check_uniform(uniform);
	glUniform2i(uniform->location, v.x, v.y);
}

void uniform_set(Uniform* uniform, const vec4& v) {
	check_uniform(uniform);
	glUniform4f(uniform->location, v.x, v.y, v.z, v.w);
}

void uniform_set(Uniform* uniform, const vec3 &v) {
	check_uniform(uniform);
	glUniform3f(uniform->location, v.x, v.y, v.z);
}

void uniform_set(Uniform* uniform, const vec2 &v) {
	check_uniform(uniform);
	glUniform2f(uniform->location, v.x, v.y);
}

void uniform_set(Uniform* uniform, float f) {
	check_uniform(uniform);
	glUniform1f(uniform->location, f);
}

void uniform_set(Uniform* uniform, int i) {
	check_uniform(uniform);
	glUniform1i(uniform->location, i);
}

}
}
}
