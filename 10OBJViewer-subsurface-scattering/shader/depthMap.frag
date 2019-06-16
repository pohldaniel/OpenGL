#version 410 core

out vec4 color;

void main(void){

	
   color =  vec4(vec3(gl_FragCoord.z), 1.0);
   
}