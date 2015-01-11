#version 130

uniform mat3 u_proj;
uniform mat3 u_model;

in vec2 in_pos;
in vec2 in_uv;

out vec2 uv;

void main() {
	vec3 w_pos = u_proj * u_model * vec3(in_pos, 1.0);
	gl_Position =  vec4(w_pos.xy, 0.0, w_pos.z);
	uv = in_uv;
}
