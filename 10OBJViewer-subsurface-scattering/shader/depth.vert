#version 410 core

layout(location = 0) in vec3 i_position;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_modelView;

out vec4 vPosLS;

void main(void){

  vPosLS = u_projection  * u_modelView * vec4(i_position, 1.0);
  vPosLS /= vPosLS.wwww;
  
  gl_Position =  vPosLS;
}