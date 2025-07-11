#version 330 core

layout (location = 0) in vec3 VertexPos;
layout (location = 1) in vec3 VertexColor;

out vec3 Color;

uniform float scr_aspect;

void main()
{ 
	gl_Position = vec4(VertexPos.x*scr_aspect, VertexPos.yz, 1.0);
	Color = VertexColor;
}