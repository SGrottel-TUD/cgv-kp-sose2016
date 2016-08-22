#version 330 core

struct Maps
{
	sampler2D normalView;
	sampler2D diffuseColor;
};

struct DirectionalLight
{
	vec3 direction;	// normalized, in view space
	vec3 diffuseColor;
};

uniform Maps maps;
uniform DirectionalLight light;

noperspective in vec2 vsTextureCoord;
out vec4 fsColor;

void main()
{
	vec3 normal = normalize(texture(maps.normalView, vsTextureCoord).xyz);

	float diffuseFactor = -dot(light.direction, normal);
	if (diffuseFactor > 0.0)
	{
		vec4 diffuseColor = texture(maps.diffuseColor, vsTextureCoord);

		// diffuse
		fsColor = diffuseColor * vec4(light.diffuseColor * diffuseFactor, 1.0);
	}
	else
	{
		fsColor = vec4(0.0, 0.0, 0.0, 0.0);
	}
}
