#version 330

uniform sampler2D sampler;

in vec2 vsTextureCoord;
out vec4 fsColor;

void main(){
	fsColor = texture(sampler, vsTextureCoord);
}