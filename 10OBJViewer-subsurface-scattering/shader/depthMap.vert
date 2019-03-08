#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_normal;

uniform mat4 u_projection;
uniform mat4 u_normalMatrix;
uniform mat4 u_modelView;



uniform float grow = 0.0;

void main(void){

  vec3 normal = (u_normalMatrix * vec4(i_normal, 0.0)).xyz;
  
  gl_Position =  u_projection  * u_modelView * vec4(i_position, 1.0);
 gl_Position.xyz += normal * grow;
  
  
  
 
}