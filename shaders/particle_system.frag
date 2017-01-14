#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D texture_sampler;

void main(){
	color = texture( texture_sampler, UV ) * vec4(1.0, 1.0, 1.0, 0.7);
}