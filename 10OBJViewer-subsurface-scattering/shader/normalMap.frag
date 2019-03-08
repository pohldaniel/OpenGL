#version 410 core

uniform sampler2D u_textureColor;
uniform sampler2D u_texture2;

in vec4 v_position;
in vec2 v_texCoord;
in vec3 v_normal;
in vec4 vPosLS;

out vec4 color;

void main(void){

	float zOutWC = ((vPosLS / vPosLS.w).z- 0.5)*2.0;
	//zOutWC = (vPosLS / vPosLS.w).z;

	 gl_FragColor =  vec4(normalize(v_normal) , 1.0);
	// gl_FragColor =  vec4(v_position , 1.0);
	 float depth = v_position.z/v_position.w;
	gl_FragColor = vec4(v_normal, 1.0);
}
