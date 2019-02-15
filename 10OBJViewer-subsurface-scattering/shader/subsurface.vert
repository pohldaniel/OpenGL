#version 410 core



layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_projection;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_normalMatrix;



uniform mat4 u_projectionShadow;
uniform mat4 u_viewShadow;
uniform mat4 u_viewOppositeShadow;

uniform mat4 u_modelViewShadow;


const mat4 bias = mat4(	0.5, 0.0, 0.0, 0.0,
						0.0, 0.5, 0.0, 0.0,
						0.0, 0.0, 0.5, 0.0,
						0.5, 0.5, 0.5, 1.0);



out vec3 v_normal;
out vec4 v_peye;
out vec4 sc;
out vec4 sc2;


void main(void){


   gl_Position =   u_projection * u_view * u_model * vec4(i_position, 1.0);
  
   //gl_Position =  u_projectionShadow * u_viewShadow * u_model * vec4(i_position, 1.0f);
   
   v_normal = (u_normalMatrix * vec4(i_normal, 0.0)).xyz;
   v_peye = u_view * u_model * vec4(i_position, 1.0);
   sc = bias * u_projectionShadow * u_viewShadow * u_model  * vec4(i_position, 1.0f);
   sc2 =  vec4(i_position, 1.0f);
}