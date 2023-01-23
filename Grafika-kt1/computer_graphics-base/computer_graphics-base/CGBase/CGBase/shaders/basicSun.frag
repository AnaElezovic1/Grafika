#version 330 core

out vec4 FragColor;

in vertex_out{
vec4 colorChannel;
}Vertex_Out;

uniform vec3 offset;

void main() {
	FragColor = vec4( Vertex_Out.colorChannel.rgb + offset.rgb, 1.0f);
}