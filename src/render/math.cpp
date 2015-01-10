#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/render/math.hpp>

namespace derpkit {
namespace render {

vec2::vec2(float x, float y) : x(x), y(y) { }
ivec2::ivec2(int x, int y) : x(x), y(y) { }
vec3::vec3(float x, float y, float z) : x(x), y(y), z(z) { }

mat3::mat3(float m00, float m01, float m02,
	       float m10, float m11, float m12,
	       float m20, float m21, float m22)
: m{
	m00, m01, m02,
	m10, m11, m12,
	m20, m21, m22
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

float dot(const vec2& v1, const vec2& v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

float dot(const vec3& v1, const vec3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

vec2 normalized(const vec2& v) {
	float n= v.length();
	return vec2(v.x / n, v.y / n);
}

vec3 normalized(const vec3& v) {
	float n= v.length();
	return vec3(v.x / n, v.y / n, v.z / n);
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

/*
mat3 mat3::operator*(const mat3& m1, const mat3& m2)
{
	mat3 ret;
	int i,j,k;
	const float* fm1 = (float*)&m1;
	const float* fm2 = (float*)&m2;
	float* fret = (float*)&ret;
	for (j=0; j<3; ++j) {
		for (i=0; i<3; ++i) {
			for(k=0; k<3; ++k) {
				fret[i*3 + j] += fm1[i * 3 + k] * fm2[k * 3 + j];
			}
		}
	}
	return ret;
}
*/

}
}
