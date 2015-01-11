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

struct Shader {
	GLuint resource;
	Uniform u_proj;
	Uniform u_model;
};

#ifdef ENABLE_DEBUG
static GLuint bound_shader = 0;
#endif

GLuint compile_shader(GLenum shaderType, const char* source) {
	const GLuint shader = glCreateShader(shaderType);

	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	GLint compile_status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

	if ( compile_status == GL_FALSE ) {
		char buffer[2048];

		Logging::error("Failed to compile shader: \n");
		std::stringstream code(source);
		int linenr=0;
		while(!code.eof()) {
			code.getline(buffer, 2048);
			Logging::error("%4d %s\n", ++linenr, buffer);
		}
		glGetShaderInfoLog(shader, 2048, NULL, buffer);
		Logging::error("Error in shader %s\n", buffer);
		return 0;
	}

	return shader;
}

GLuint link_program(const std::string &shader_name, const std::vector<GLuint> &shaderList) {
	GLint gl_tmp;
	GLuint program = glCreateProgram();

	for(GLuint shader : shaderList) {
		glAttachShader(program, shader);
	}

	glLinkProgram(program);

	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

	glGetProgramiv(program, GL_LINK_STATUS, &gl_tmp);

	if(!gl_tmp) {
		char buffer[2048];
		glGetProgramInfoLog(program, 2048, NULL, buffer);
		Logging::error("Link error in shader %s: %s\n", shader_name.c_str(), buffer);
		return 0;
	}

#ifdef ENABLE_DEBUG
	glValidateProgram(program);

	glGetProgramiv(program, GL_VALIDATE_STATUS, &gl_tmp);

	if(!gl_tmp) {
		char buffer[2048];
		glGetProgramInfoLog(program, 2048, NULL, buffer);
		Logging::error("Validate error in shader %s: %s\n", shader_name.c_str(), buffer);
		return 0;
	}

#endif

	return program;
}

void free_shader(Shader* shader) {
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
	Uniform* tmp = get_uniform(shader, "u_proj");
	shader->u_proj = *tmp;
	free_uniform(tmp);

	tmp = get_uniform(shader, "u_model");
	shader->u_model = *tmp;
	free_uniform(tmp);
}

#ifdef ENABLE_DEBUG
Shader* create_shader_from_filename(const std::string& filename) {
	std::vector<GLuint> shader_list;
	{
		FileData file = load_file(filename + ".vert");
		shader_list.push_back(compile_shader(GL_VERTEX_SHADER, file.data()));
	}
	{
		FileData file = load_file(filename + ".frag");
		shader_list.push_back(compile_shader(GL_FRAGMENT_SHADER, file.data()));
	}

	for(GLuint i : shader_list) {
		if(i == 0) return nullptr;
	}

	GLuint resource = link_program(filename, shader_list);
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

void uniform_set(Uniform* uniform, const css::Color &v) {
	check_uniform(uniform);
	glUniform4f(uniform->location, v.r, v.g, v.b, v.a);
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

void DERPKIT_DEBUG_EXPORT set_projection(Shader* shader, const mat3& m) {
	// TODO: Maybe cache which proj matrix resolution is bound, to prevent reupload every time
	uniform_set(&shader->u_proj, m);
}

void DERPKIT_DEBUG_EXPORT set_model_matrix(Shader* shader, const mat3& m) {
	uniform_set(&shader->u_model, m);
}


}
}
}
