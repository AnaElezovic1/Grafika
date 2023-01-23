#version 330 core

out vec4 FragColor;

in vertex_out{
vec3 colorChannel;
}Vertex_Out;

void main() {
	FragColor = vec4( Vertex_Out.colorChannel, 1.0f);
}