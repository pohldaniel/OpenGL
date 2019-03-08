#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_normal;

uniform mat4 u_projection;
uniform mat4 u_normalMatrix;
uniform mat4 u_modelView;



out vec4 v_pos;


void main(void){

  gl_Position =  u_projection  * u_modelView * vec4(i_position, 1.0);
  v_pos = gl_Position;
}