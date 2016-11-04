#version 330 core

uniform mat4 projection;

layout (location = 0) in vec3 inPosition;
layout (location = 2) in vec2 inTextureCoord;
layout (location = 3) in vec4 wv0;
layout (location = 4) in vec4 wv1;
layout (location = 5) in vec4 wv2;
layout (location = 6) in vec4 wv3;
out vec3 vsPositionView;
out vec2 vsTextureCoord;

void main()
{
	vec4 positionView = mat4(wv0, wv1, wv2, wv3) * vec4(inPosition, 1.0);
	gl_Position = projection * positionView;
	vsPositionView = positionView.xyz;
	vsTextureCoord = inTextureCoord;
}
