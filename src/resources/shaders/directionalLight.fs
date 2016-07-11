#version 330 core

struct Maps
{
	sampler2D positionView;
	sampler2D normalView;
	sampler2D diffuseColor;
	sampler2D material;
};

struct DirectionalLight
{
	vec3 direction;	// normalized, in view space
	vec3 diffuseColor;
};

uniform Maps maps;
uniform DirectionalLight light;

in vec2 vsTextureCoord;
out vec3 fsColor;

void main()
{
	fsColor = vec3(0.0, 0.0, 0.0);

	vec3 position = texture(maps.positionView, vsTextureCoord).xyz;
	vec3 normal = normalize(texture(maps.normalView, vsTextureCoord).xyz);
	vec3 diffuseColor = texture(maps.diffuseColor, vsTextureCoord).xyz;
	vec2 material = texture(maps.material, vsTextureCoord).xy;    // = vec2(specularPower, specularIntensity)

	float diffuseFactor = -dot(light.direction, normal);
	if (diffuseFactor > 0.0)
	{
		// diffuse
		fsColor += diffuseColor * light.diffuseColor * diffuseFactor;

		// specular
		float specularFactor = -dot(normalize(position), reflect(light.direction, normal));
		if(specularFactor > 0.0)
		{
			specularFactor = pow(specularFactor, material.x);
			fsColor += vec3(light.diffuseColor * material.y * specularFactor);
		}
	}
}
