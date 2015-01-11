#version 130

uniform sampler2D texture0;

in vec2 uv;

out vec4 color;

void main() {
	color = vec4(texture2D(texture0, uv).rgb , 1.f);
}
