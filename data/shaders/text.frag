#version 130
#pragma stage fragment
#pragma sampler texture0:0

uniform sampler2D texture0;
uniform vec4 color = vec4(1.f);

in vec2 uv;

out vec4 out_color;

void main() {
	out_color = vec4(1.0, 1.0, 1.0, texture2D(texture0, uv).r) * color;
}
