#version 330 core
in vec3 frag_position;
in vec3 frag_normal;
out vec4 o_color;

void main() {
    o_color = vec4(frag_position + frag_normal, 1.0);
}