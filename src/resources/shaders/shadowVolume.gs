#version 330

layout (triangles_adjacency) in;
layout (triangle_strip, max_vertices = 18) out;

in vec3 positionWorld[];
in vec3 normalWorld[];

uniform vec3 lightPosition;
uniform mat4 viewProjection;

float bias = 0.0002;

// Emit a quad using a triangle strip
void emitQuad(int startIndex, int endIndex)
{
	// Vertex #1: the starting vertex (just a tiny bit below the original edge)
	vec3 lightDir = normalize(positionWorld[startIndex] - lightPosition);
	gl_Position = viewProjection * vec4(positionWorld[startIndex] + (lightDir - normalWorld[startIndex]) * bias, 1.0);
	EmitVertex();

	// Vertex #2: the starting vertex projected to infinity
	gl_Position = viewProjection * vec4(lightDir, 0.0);
	EmitVertex();

	// Vertex #3: the ending vertex (just a tiny bit below the original edge)
	lightDir = normalize(positionWorld[endIndex] - lightPosition);
	gl_Position = viewProjection * vec4(positionWorld[endIndex] + (lightDir - normalWorld[endIndex]) * bias, 1.0);
	EmitVertex();

	// Vertex #4: the ending vertex projected to infinity
	gl_Position = viewProjection * vec4(lightDir , 0.0);
	EmitVertex();

	EndPrimitive();
}


void main()
{
	vec3 edge0to1 = positionWorld[1] - positionWorld[0];
	vec3 edge0to2 = positionWorld[2] - positionWorld[0];
	vec3 edge0to4 = positionWorld[4] - positionWorld[0];
	vec3 edge0to5 = positionWorld[5] - positionWorld[0];
	vec3 edge2to3 = positionWorld[3] - positionWorld[2];
	vec3 edge2to4 = positionWorld[4] - positionWorld[2];

	vec3 normal = cross(edge0to2, edge0to4);
	vec3 lightDir = lightPosition - positionWorld[0];

	// Handle only light facing triangles
	if (dot(normal, lightDir) > 0.0)
	{
		normal = cross(edge0to1, edge0to2);
		if (dot(normal, lightDir) <= 0.0)
		{
			emitQuad(0, 2);
		}

		normal = cross(edge2to3, edge2to4);
		lightDir = lightPosition - positionWorld[2];
		if (dot(normal, lightDir) <= 0.0)
		{
			emitQuad(2, 4);
		}

		normal = cross(edge0to4, edge0to5);
		lightDir = lightPosition - positionWorld[4];
		if (dot(normal, lightDir) <= 0.0)
		{
			emitQuad(4, 0);
		}

		// render the front cap
		lightDir = normalize(positionWorld[0] - lightPosition);
		gl_Position = viewProjection * vec4(positionWorld[0] + (lightDir - normalWorld[0]) * bias, 1.0);
		EmitVertex();

		lightDir = normalize(positionWorld[2] - lightPosition);
		gl_Position = viewProjection * vec4(positionWorld[2] + (lightDir - normalWorld[2]) * bias, 1.0);
		EmitVertex();

		lightDir = normalize(positionWorld[4] - lightPosition);
		gl_Position = viewProjection * vec4(positionWorld[4] + (lightDir - normalWorld[4]) * bias, 1.0);
		EmitVertex();
		EndPrimitive();

		// render the back cap
		lightDir = positionWorld[0] - lightPosition;
		gl_Position = viewProjection * vec4(lightDir, 0.0);
		EmitVertex();

		lightDir = positionWorld[4] - lightPosition;
		gl_Position = viewProjection * vec4(lightDir, 0.0);
		EmitVertex();

		lightDir = positionWorld[2] - lightPosition;
		gl_Position = viewProjection * vec4(lightDir, 0.0);
		EmitVertex();

		EndPrimitive();
	}
}
