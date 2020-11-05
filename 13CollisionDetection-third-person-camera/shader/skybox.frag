#version 410 core

uniform samplerCube u_skybox;

in vec3 v_texCoord;


out vec4 color;

void main(void){
	
	color = texture( u_skybox, v_texCoord );
	
	
}
