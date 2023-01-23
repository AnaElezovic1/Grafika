#version 330 core

uniform vec3 offset;
out vec4 FragColor;

void main() {
	FragColor = vec4(offset.rgb, 1.0f);
}