#version 330 core

const int numDirections = 6;
const int numSteps = 4;
const float radius = 0.01;

struct Maps
{
	sampler2D positionView;
	sampler2D normalView;
	sampler2D diffuseColor;
};

uniform Maps maps;
uniform vec3 randomDirections[6];	// must be in a unit radius sphere
uniform vec3 ambientLight;

noperspective in vec2 vsTextureCoord;
out vec4 fsColor;

float rand(vec2 v)
{
	return fract(sin(dot(v, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
	vec3 normal = normalize(texture(maps.normalView, vsTextureCoord).xyz);
	float depth = texture(maps.positionView, vsTextureCoord).z;
	vec4 diffuseColor = texture(maps.diffuseColor, vsTextureCoord);
	
	float distancePerStep = rand(vsTextureCoord);
	//float distancePerStep = 0.5;

	float occlusion = 0.0;
	for (int i = 0; i < numDirections; ++i)
	{
		vec3 direction = normalize(randomDirections[i]);
		if (dot(normal, direction) < 0.0)
		{
			direction = -direction;
		}

		float distance = 0.0;
		for (int j = 0; j < numSteps; ++j)
		{
			distance += distancePerStep;
			vec3 delta = direction * distance * radius;
			float sampledDepth = texture(maps.positionView, vsTextureCoord + delta.xy).z;

			if (sampledDepth - depth >= 0.0)
			{
				occlusion += 1;//numSteps / (1.0 + weight * weight);
			}
		}
	}

	float occlusionAmount = 1.0 - occlusion / (numSteps * numDirections);
	fsColor = diffuseColor * occlusionAmount * vec4(ambientLight, 1.0);
}
