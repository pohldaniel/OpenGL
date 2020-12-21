#version 410 core

uniform sampler2D u_texture;


in vec3 v_color;
in vec2 v_texCoord;

out vec4 color;

void main(void){

	color = texture2D( u_texture, v_texCoord ) ;
	
	
}
