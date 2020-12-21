#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;

out vec2 v_texCoord;
out vec3 v_color;

void main(void){
	
   gl_Position = vec4(i_position, 1.0);
   v_texCoord = i_texCoord;
}