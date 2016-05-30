#version 330 core

uniform mat4 modelMatrix;
uniform mat4 worldViewProjection;

layout(location = 0) vert_position;
layout(location = 1) vert_normal;
out vec3 frag_position;
out vec3 frag_normal;

void main() {
    frag_position = abs(vert_position);
    frag_normal = abs(vert_normal);
	vec4 pos_world = modelMatrix * vec4(vert_position, 1.0);
    gl_Position = worldViewProjection * pos_world;
}