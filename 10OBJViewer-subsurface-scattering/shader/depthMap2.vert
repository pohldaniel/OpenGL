#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_normal;

uniform mat4 u_projection;
uniform mat4 u_normalMatrix;
uniform mat4 u_modelView;

uniform mat4 u_mvp;

uniform mat4 u_mvp2;

uniform float u_float;

out vec4 v_position;
out float tmp;

out float w;

vec4  position;

void main(void){

  position = u_mvp * vec4(i_position, 1.0);
  v_position = position ; 
  gl_Position = position ;
  
  
}