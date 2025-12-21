#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TextureCoord;

uniform vec3 LightPos;
uniform vec3 CameraPos;
uniform vec3 AmbientLight;
uniform vec3 LightColor;
uniform sampler2D Texture;

out vec4 FragColor;

void main()
{
	vec3 Color = texture(Texture, TextureCoord).xyz;

	vec3 AmbientColor = AmbientLight * Color;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(LightPos-FragPos);
	vec3 DiffuseColor = max(0.0f, dot(norm, lightDir))*LightColor*Color;

	vec3 viewDir = normalize(CameraPos-FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 SpecularColor = 0.7*pow(max(0.0f, dot(reflectDir, viewDir)), 64)*LightColor;

	FragColor = vec4(AmbientColor+DiffuseColor+SpecularColor, 1.0f);
}