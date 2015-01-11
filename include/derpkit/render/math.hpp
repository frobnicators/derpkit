#ifndef DERPKIT_RENDER_MATH_HPP
#define DERPKIT_RENDER_MATH_HPP

#include <cmath>

namespace derpkit {
namespace render {

struct DERPKIT_EXPORT vec2 {
	vec2() {};
	vec2(float x, float y);

	float x;
	float y;

	float length() const;

	vec2 operator*(float f) const;
	vec2 operator*(const vec2& v) const;
	vec2 operator+(const vec2& v) const;
	vec2 operator-(const vec2& v) const;
};

struct DERPKIT_EXPORT ivec2 {
	ivec2() {};
	ivec2(int x, int y);

	int x;
	int y;

	float length() const;
};

struct DERPKIT_EXPORT vec3 {
	vec3() {};
	vec3(float x, float y, float z);

	union {
		struct {
			float x;
			float y;
			float z;
		};
		struct {
			float r;
			float g;
			float b;
		};
	};

	float length() const;
};

struct DERPKIT_EXPORT vec4 {
	vec4() {};
	vec4(float x, float y, float z, float w);

	union {
		struct {
			float x;
			float y;
			float z;
			float w;
		};
		struct {
			float r;
			float g;
			float b;
			float a;
		};
	};

	float length() const;
};

struct DERPKIT_EXPORT mat3 {
	mat3() {};
	mat3(float m00, float m01, float m02,
		 float m10, float m11, float m12,
		 float m20, float m21, float m22);
	mat3(const mat3& m);

	float m[9];
};

inline const float* value_ptr(const vec2& v) { return reinterpret_cast<const float*>(&v); }
inline const float* value_ptr(const vec3& v) { return reinterpret_cast<const float*>(&v); }
inline const float* value_ptr(const vec4& v) { return reinterpret_cast<const float*>(&v); }
inline const float* value_ptr(const mat3& m) { return m.m; }

float DERPKIT_EXPORT dot(const vec2& v1, const vec2& v2);
float DERPKIT_EXPORT dot(const vec3& v1, const vec3& v2);
float DERPKIT_EXPORT dot(const vec4& v1, const vec4& v2);

vec2 DERPKIT_EXPORT normalized(const vec2& v);
vec3 DERPKIT_EXPORT normalized(const vec3& v);
vec4 DERPKIT_EXPORT normalized(const vec4& v);

mat3 DERPKIT_EXPORT ortho(const ivec2& size);
mat3 DERPKIT_EXPORT model_matrix(const vec2& pos, const vec2& size);

}
}

#endif
