#version 330 core

layout (location = 0) in vec3 VertexPos;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 Normal;
out vec3 FragPos;
out vec2 TextureCoord;

void main()
{ 
	gl_Position = proj * view * model * vec4(VertexPos, 1.0);
	Normal = mat3(transpose(inverse(model))) * VertexNormal;
	FragPos = vec3(model * vec4(VertexPos, 1.0f));
	TextureCoord = VertexTexCoord;
}