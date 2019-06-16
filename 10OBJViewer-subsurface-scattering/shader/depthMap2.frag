#version 410 core

in vec4 v_position;
in float tmp;
in float w;
//out vec3 color;

out float color;

void main(void){

	//color = vec3(0.5, 0.5, 0.5);

	
   //color = gl_FragCoord.z *gl_FragCoord.w;
   
   color = v_position.z;
}