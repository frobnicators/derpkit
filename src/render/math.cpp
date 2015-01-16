#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/render/math.hpp>
#include <cstring>
#include <cmath>

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

mat3& mat3::operator=(const mat3& mat) {
	memcpy(m, mat.m, sizeof(float)*9);
	return *this;
}

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
	float SrcA00 = row[0][0];
	float SrcA01 = row[0][1];
	float SrcA02 = row[0][2];
	float SrcA10 = row[1][0];
	float SrcA11 = row[1][1];
	float SrcA12 = row[1][2];
	float SrcA20 = row[2][0];
	float SrcA21 = row[2][1];
	float SrcA22 = row[2][2];

	float SrcB00 = mat.row[0][0];
	float SrcB01 = mat.row[0][1];
	float SrcB02 = mat.row[0][2];
	float SrcB10 = mat.row[1][0];
	float SrcB11 = mat.row[1][1];
	float SrcB12 = mat.row[1][2];
	float SrcB20 = mat.row[2][0];
	float SrcB21 = mat.row[2][1];
	float SrcB22 = mat.row[2][2];

	mat3 ret;
	ret.row[0][0] = SrcA00 * SrcB00 + SrcA10 * SrcB01 + SrcA20 * SrcB02;
	ret.row[0][1] = SrcA01 * SrcB00 + SrcA11 * SrcB01 + SrcA21 * SrcB02;
	ret.row[0][2] = SrcA02 * SrcB00 + SrcA12 * SrcB01 + SrcA22 * SrcB02;
	ret.row[1][0] = SrcA00 * SrcB10 + SrcA10 * SrcB11 + SrcA20 * SrcB12;
	ret.row[1][1] = SrcA01 * SrcB10 + SrcA11 * SrcB11 + SrcA21 * SrcB12;
	ret.row[1][2] = SrcA02 * SrcB10 + SrcA12 * SrcB11 + SrcA22 * SrcB12;
	ret.row[2][0] = SrcA00 * SrcB20 + SrcA10 * SrcB21 + SrcA20 * SrcB22;
	ret.row[2][1] = SrcA01 * SrcB20 + SrcA11 * SrcB21 + SrcA21 * SrcB22;
	ret.row[2][2] = SrcA02 * SrcB20 + SrcA12 * SrcB21 + SrcA22 * SrcB22;
	return ret;
}

Transform::Transform()
: rotation(0.f)
{ }


Transform::Transform(float rotation)
: rotation(rotation)
{ }

Transform::Transform(const vec2& pos, float rotation, const vec2& scale)
: position(pos)
, rotation(rotation)
, scale(scale)
{ }

mat3 Transform::matrix() const {
	return mat3(
		cos(rotation) * scale.x, -sin(rotation), 0.f,
		sin(rotation), cos(rotation) * scale.y , 0.f,
		position.x , position.y                , 1.f);
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
