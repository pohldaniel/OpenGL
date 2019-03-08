#version 410 core

in vec4 v_pos;

//out vec3 color;

out float color;

void main(void){

	//color = vec3(0.5, 0.5, 0.5);

	
   color = (v_pos.z/v_pos.w)*0.5 + 0.5;
}