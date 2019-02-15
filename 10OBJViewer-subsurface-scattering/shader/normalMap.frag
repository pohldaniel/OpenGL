#version 410 core

uniform sampler2D u_textureColor;
uniform sampler2D u_texture2;


in vec2 v_texCoord;
in vec3 v_normal;
in vec4 vPosLS;

out vec4 color;

void main(void){

	 gl_FragColor =  vec4(v_normal * 0.5 + 0.5, 1.0 );
	 
	//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
