#version 330 core

struct Maps
{
	sampler2D positionWorld;
	sampler2D normalWorld;
	sampler2D diffuseColor;
	sampler2D material;
};

struct PointLight
{
	vec3 position;
	vec3 diffuseColor;
	vec3 attenuation;	// = vec3(constant, linear, exponential)
};

uniform Maps maps;
uniform PointLight light;
uniform ivec2 screenSize;
uniform vec3 eyePosition;

layout(location = 0) out vec4 fsColor;

vec3 calculateLight(vec3 positionWorld, vec3 normalWorld, vec2 material)    // material = vec2(specularPower, specularIntensity)
{
	vec3 lightDirection = positionWorld - light.position;
	float distance = length(lightDirection);
	lightDirection = normalize(lightDirection);

	float diffuseFactor = -dot(normalWorld, lightDirection);
	if (diffuseFactor > 0.0)
	{
		// diffuse
		vec3 color = light.diffuseColor * diffuseFactor;

		// specular
		vec3 vertexToEye = normalize(eyePosition - positionWorld);
		vec3 lightReflect = normalize(reflect(lightDirection, normalWorld));
		float specularFactor = dot(vertexToEye, lightReflect);
		if(specularFactor > 0.0)
		{
			specularFactor = pow(specularFactor, material.x);
			color += vec3(light.diffuseColor * material.y * specularFactor);
		}

		float attenuation = dot(light.attenuation, vec3(1.0, distance, distance * distance));
		attenuation = max(1.0, attenuation);

		return color / attenuation;
	}

	return vec3(0.0, 0.0, 0.0);
}

void main()
{
	vec2 coord = gl_FragCoord.xy / screenSize;

	vec3 positionWorld = texture(maps.positionWorld, coord).xyz;
	vec3 normalWorld = normalize(texture(maps.normalWorld, coord).xyz);
	vec2 material = texture(maps.material, coord).xy;    // vec2(specularPower, specularIntensity)

	fsColor = vec4(texture(maps.diffuseColor, coord).xyz * calculateLight(positionWorld, normalWorld, material), 1.0);
}
