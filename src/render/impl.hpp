#ifndef DERPKIT_RENDERTARGETIMPL_HPP
#define DERPKIT_RENDERTARGETIMPL_HPP

#include <derpkit/render/math.hpp>
#include <derpkit/css/datatype.hpp>
#include <string>

namespace derpkit {
namespace render {

namespace impl {

// Declarations

struct RenderTarget;
struct Shader;
struct Uniform;

// General

void initialize();
void cleanup();

void check_for_errors(const char* context);

// Drawing

void clear();
void draw_rect();

// RenderTargets

RenderTarget* create_rendertarget(const ivec2& resolution);
void free_rendertarget(RenderTarget* target);

void bind_rendertarget(RenderTarget* target);
void unbind_rendertarget();

// Shaders

void free_shader(Shader* shader);
void bind_shader(Shader* shader);
void unbind_shader();

#ifdef ENABLE_DEBUG
Shader* create_shader_from_filename(const std::string& filename);
#endif

// Uniforms

// The shader must be bound
void set_projection(Shader* shader, const mat3& m);
void set_model_matrix(Shader* shader, const mat3& m);

Uniform* get_uniform(Shader* shader, const std::string& name);
void free_uniform(Uniform* uniform);

// These requires the correct shader to be set
void uniform_set(Uniform* uniform, const ivec2 &v);
void uniform_set(Uniform* uniform, const vec3 &v);
void uniform_set(Uniform* uniform, const css::Color &color);
void uniform_set(Uniform* uniform, const vec2 &v);
void uniform_set(Uniform* uniform, const mat3 &m);
void uniform_set(Uniform* uniform, float f);
void uniform_set(Uniform* uniform, int i);

}

}
}

#endif
