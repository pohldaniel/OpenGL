#version 410 core

uniform sampler2D u_textureColor;
uniform sampler2D u_texture2;

in vec3 v_color;
in vec2 v_texCoord;

out vec4 color;

void main(void){

	color = texture2D( u_textureColor, v_texCoord ) ;
	//color = texture2D( u_texture2, v_texCoord ) ;
	
}
