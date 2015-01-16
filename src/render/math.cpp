#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/render/math.hpp>
#include <cstring>

namespace derpkit {
namespace render {

vec2::vec2(float v) : x(v), y(v) { }
vec2::vec2(float x, float y) : x(x), y(y) { }
ivec2::ivec2(int v) : x(v), y(v) { }
ivec2::ivec2(int x, int y) : x(x), y(y) { }
vec3::vec3(float x, float y, float z) : x(x), y(y), z(z) { }
vec3::vec3(float v) : x(v), y(v), z(v) { }
vec4::vec4(float v) : x(v), y(v), z(v), w(v) { }
vec4::vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { }
box::box() : x(0.f), y(0.f), w(0.f), h(0.f) { }
box::box(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) { }

mat3::mat3(float m00, float m01, float m02,
	       float m10, float m11, float m12,
	       float m20, float m21, float m22)
: m{
	m00, m01, m02,
	m10, m11, m12,
	m20, m21, m22
	}
{ }

mat3::mat3() : m{
	1.f, 0.f, 0.f,
	0.f, 1.f, 0.f,
	0.f, 0.f, 1.f
   }
{ }

vec2 vec2::operator*(float f) const {
	return vec2(x*f, y*f);
}

vec2 vec2::operator*(const vec2& v) const {
	return vec2(x*v.x, y*v.y);
}

vec2 vec2::operator+(const vec2& v) const {
	return vec2(x+v.x, y+v.y);
}

vec2 vec2::operator-(const vec2& v) const {
	return vec2(x-v.x, y-v.y);
}

float vec2::length() const {
	return (float)sqrt(dot(*this, *this));
}

float vec3::length() const {
	return (float)sqrt(dot(*this, *this));
}

float vec4::length() const {
	return (float)sqrt(dot(*this, *this));
}

float dot(const vec2& v1, const vec2& v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

float dot(const vec3& v1, const vec3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float dot(const vec4& v1, const vec4& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

vec2 normalized(const vec2& v) {
	float n= v.length();
	return vec2(v.x / n, v.y / n);
}

vec3 normalized(const vec3& v) {
	float n= v.length();
	return vec3(v.x / n, v.y / n, v.z / n);
}

vec4 normalized(const vec4& v) {
	float n = v.length();
	return vec4(v.x / n, v.y / n, v.z / n, v.w / n);
}

mat3 ortho(const ivec2& size) {
	float fx = static_cast<float>(size.x);
	float fy = static_cast<float>(size.y);

	return mat3(
		2.f/fx , 0.f    , 0.f,
		0.f    , -2.f/fy , 0.f,
		-1.f    , 1.f    , 1.f);
}

mat3 model_matrix(const vec2& pos, const vec2& size) {
	return mat3(
		size.x, 0.f   , 0.f,
		0.f   , size.y, 0.f,
		pos.x , pos.y , 1.f);
}

mat3 model_matrix(const box& box){
	return mat3(
		box.w, 0.0f,  0.0f,
		0.0f,  box.h, 0.0f,
		box.x, box.y, 1.0f
	);
}

mat3 mat3::operator*(const mat3& mat) const {
	mat3 ret;
	int i,j,k;
	const float* fm1 = m;
	const float* fm2 = mat.m;
	float* fret = ret.m;
	memset(fret, 0, 9*sizeof(float));
	for (j=0; j<3; ++j) {
		for (i=0; i<3; ++i) {
			for(k=0; k<3; ++k) {
				fret[i*3 + j] += fm1[i * 3 + k] * fm2[k * 3 + j];
			}
		}
	}
	return ret;
}

Transform::Transform()
: m_has_position(false)
, m_has_rotation(false)
, m_has_scale(false)
, m_rotation(0.f)
{ }

Transform::Transform(const vec2& pos)
: m_has_position(true)
, m_has_rotation(false)
, m_has_scale(false)
, m_position(pos)
, m_rotation(0.f)
{ }


Transform::Transform(float rotation)
: m_has_position(false)
, m_has_rotation(true)
, m_has_scale(false)
, m_rotation(rotation)
{ }

Transform::Transform(const vec2& pos, float rotation)
: m_has_position(true)
, m_has_rotation(true)
, m_has_scale(false)
, m_position(pos)
, m_rotation(rotation)
{ }

Transform::Transform(const vec2& pos, float rotation, const vec2& scale)
: m_has_position(true)
, m_has_rotation(true)
, m_has_scale(true)
, m_position(pos)
, m_rotation(rotation)
, m_scale(scale)
{ }

void Transform::set_position(const vec2& pos) {
	m_position = pos;
	m_has_position = true;
}

void Transform::set_rotation(float rot) {
	m_rotation = rot;
	m_has_rotation = true;
}

void Transform::set_scale(const vec2& scale) {
	m_scale = scale;
	m_has_scale = true;
}

#ifdef ENABLE_DEBUG
DERPKIT_EXPORT std::ostream& operator<<(std::ostream& os, const vec3& v) {
	os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return os;
}

DERPKIT_EXPORT std::ostream& operator<<(std::ostream& os, const mat3& m) {
	for(int i=0; i<3; ++i) {
		os << m.row[i] << std::endl;
	}
	return os;
}
#endif

}
}
