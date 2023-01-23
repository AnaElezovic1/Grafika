# version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aCol;

out vertex_out{
vec4 colorChannel;
}Vertex_Out;

uniform mat4 uMVP;


void main() {
	Vertex_Out.colorChannel = vec4(aCol);
	gl_Position = uMVP * vec4(aPos, 1.0f);
}