#version 410 core



layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_projection;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_normalMatrix;
uniform mat4 u_modelView;


uniform mat4 u_projectionShadow;
uniform mat4 u_viewShadow;
uniform mat4 u_modelViewShadow;






out vec3 v_normal;
out vec4 v_peye;
out vec4 sc;



void main(void){


   gl_Position =   u_projection * u_view * u_model * vec4(i_position, 1.0);
  
   //gl_Position =  u_projectionShadow * u_viewShadow * u_model * vec4(i_position, 1.0f);
   
   v_normal = (u_normalMatrix * vec4(i_normal, 0.0)).xyz;
   v_peye = u_view * u_model * vec4(i_position, 1.0);
   sc = u_projectionShadow * u_viewShadow * u_model * vec4(i_position, 1.0f);
   
}