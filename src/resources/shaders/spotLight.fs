#version 330

struct Maps
{
	sampler2D positionView;
	sampler2D normalView;
	sampler2D diffuseColor;
};

struct SpotLight
{
	vec3 color;
	vec3 position;
	vec3 direction;
	vec3 attenuation;	// = vec3(constant, linear, exponential)
	float cutoff;
};

uniform Maps maps;
uniform SpotLight light;

noperspective in vec2 vsTextureCoord;
out vec3 fsColor;

void main()
{
	vec3 position = texture(maps.positionView, vsTextureCoord).xyz;
	vec3 normal = normalize(texture(maps.normalView, vsTextureCoord).xyz);
	vec3 diffuseColor = texture(maps.diffuseColor, vsTextureCoord).rgb;

	vec3 lightToFragment = position - light.position;
	float distance = length(lightToFragment);
	lightToFragment = normalize(lightToFragment);
	
	float spotFactor = dot(lightToFragment, light.direction);
	float diffuseFactor = -dot(lightToFragment, normal);
	if (spotFactor > light.cutoff && diffuseFactor > 0.0)
	{
		// diffuse
		fsColor = light.color * diffuseFactor;
		float attenuation = dot(light.attenuation, vec3(1.0, distance, distance * distance));
		fsColor *= diffuseColor / max(1.0, attenuation) * ((spotFactor - light.cutoff) / (1.0 - light.cutoff));
	}
	else
	{
		fsColor = vec3(0.0, 0.0, 0.0);
	}
}
