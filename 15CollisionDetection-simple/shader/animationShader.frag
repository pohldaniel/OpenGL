#version 410 core

in vec2 v_texCoord;
in vec3 v_normal;

uniform sampler2D diffuse;

void main(){

    gl_FragColor = texture2D(diffuse, v_texCoord);
	//gl_FragColor = vec4(v_normal, 1.0);
}