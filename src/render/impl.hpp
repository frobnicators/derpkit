#ifndef DERPKIT_IMPL_HPP
#define DERPKIT_IMPL_HPP

#include <derpkit/render/math.hpp>
#include <string>
#include <vector>

namespace derpkit {
namespace render {

namespace impl {

// Declarations

struct RenderTarget;
struct Shader;
struct ShaderStage;
struct Uniform;
struct Texture2D;

// General

void initialize();
void cleanup();

void check_for_errors(const char* context);

// Drawing

void clear(const vec4& color);
void draw_rect();

// RenderTargets

RenderTarget* create_rendertarget(const ivec2& resolution);
void free_rendertarget(RenderTarget* target);

void bind_rendertarget(RenderTarget* target);
void unbind_rendertarget();

// Shaders

enum ShaderStageId {
	ShaderStage_Vertex,
	ShaderStage_Fragment
};

ShaderStage* create_shaderstage_from_source(ShaderStageId stage, const char* name, const char* source);
void free_shaderstage(ShaderStage* stage);

Shader* create_shader(const char* name, const std::vector<ShaderStage*>& stages);
void free_shader(Shader* shader);

void bind_shader(Shader* shader);
void unbind_shader();

#ifdef ENABLE_DEBUG
Shader* create_shader_from_filename(const std::string& filename);
#endif

// Uniforms

Uniform* get_uniform(Shader* shader, const std::string& name);
Uniform* copy_uniform(const Uniform* uniform);
void free_uniform(Uniform* uniform);

// These requires the correct shader to be set
void uniform_set(Uniform* uniform, const ivec2& v);
void uniform_set(Uniform* uniform, const vec3& v);
void uniform_set(Uniform* uniform, const vec4& v);
void uniform_set(Uniform* uniform, const vec2& v);
void uniform_set(Uniform* uniform, const mat3& m);
void uniform_set(Uniform* uniform, float f);
void uniform_set(Uniform* uniform, int i);

// Textures
Texture2D* load_texture(const std::string& path);
void free_texture(Texture2D* texture);

void bind_texture(Texture2D* tex, int unit);
void unbind_texture(int unit);

}

}
}

#endif
