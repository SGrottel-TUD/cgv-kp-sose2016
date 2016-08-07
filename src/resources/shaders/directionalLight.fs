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

noperspective in vec2 vsTextureCoord;
out vec4 fsColor;

void main()
{
	fsColor = vec4(0.0, 0.0, 0.0, 0.0);

	vec3 normal = normalize(texture(maps.normalView, vsTextureCoord).xyz);

	float diffuseFactor = -dot(light.direction, normal);
	if (diffuseFactor > 0.0)
	{
		vec3 position = texture(maps.positionView, vsTextureCoord).xyz;
		vec4 diffuseColor = texture(maps.diffuseColor, vsTextureCoord);

		// diffuse
		fsColor += diffuseColor * vec4(light.diffuseColor * diffuseFactor, 1.0);

		// specular
		float specularFactor = -dot(normalize(position), reflect(light.direction, normal));
		if(specularFactor > 0.0)
		{
			vec2 material = texture(maps.material, vsTextureCoord).xy;    // = vec2(specularPower, specularIntensity)

			specularFactor = pow(specularFactor, material.x);
			fsColor += vec4(light.diffuseColor * material.y * specularFactor, 0.0);
		}
	}
}
