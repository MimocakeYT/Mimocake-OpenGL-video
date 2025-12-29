#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TextureCoord;

uniform vec3 AmbientLight;

struct PointLight
{
	vec3 pos;
	vec3 col;
};
uniform PointLight pl[3];

struct DirectionalLight
{
	vec3 dir;
	vec3 col;
};
uniform DirectionalLight dl;

struct SpotLight
{
	vec3 pos;
	vec3 spotDir;
	float angle;
	float outerAngle;
	vec3 col;
};
uniform SpotLight sl[3];

uniform vec3 CameraPos;
uniform sampler2D Texture;

out vec4 FragColor;

vec3 CalcDiffuseLight(vec3 LightCol, vec3 LightDir, vec3 N)
{
	return max(0.0f, dot(normalize(N), normalize(LightDir)))*LightCol;
}

vec3 CalcSpecularLight(vec3 LightCol, vec3 LightDir, vec3 N, vec3 ViewDir)
{
	vec3 reflDir = normalize(reflect(-LightDir, N));
	return pow(max(0.0f, dot(normalize(ViewDir), reflDir)), 64)*LightCol;
}

void main()
{
	vec3 Color = texture(Texture, TextureCoord).xyz;
	vec3 result = AmbientLight * Color;

	// Directional light
	vec3 Diffuse = CalcDiffuseLight(dl.col, dl.dir, Normal);
	vec3 Specular = 0.7*CalcSpecularLight(dl.col, dl.dir, Normal, CameraPos-FragPos);
	result += (Diffuse+Specular) * Color;

	// Point light
	for (int i = 0; i < 3; i++) {
		vec3 lightDir = pl[i].pos-FragPos;
		float d = length(lightDir);
		float a = 1 / (1 + 0.1*d + 0.03*d*d);

		Diffuse = CalcDiffuseLight(pl[i].col, lightDir, Normal);
		Specular = 0.7 * CalcSpecularLight(pl[i].col, lightDir, Normal, CameraPos-FragPos);
		result += a * (Diffuse+Specular) * Color;
	}

	// Spotlight
	for (int i = 0; i < 3; i++) {
		vec3 lightDir = sl[i].pos-FragPos;
		float d = length(lightDir);
		float phi = dot(normalize(sl[i].spotDir), normalize(-lightDir));

		float intensity = clamp((sl[i].outerAngle - phi)/(sl[i].outerAngle - sl[i].angle), 0, 1);
		float a = 1 / (1 + 0.1*d + 0.03*d*d);

		Diffuse = CalcDiffuseLight(sl[i].col, lightDir, Normal);
		Specular = 0.7 * CalcSpecularLight(sl[i].col, lightDir, Normal, CameraPos-FragPos);
		result += a * intensity * (Diffuse+Specular) * Color;
	}

	FragColor = vec4(result, 1.0f);
}

