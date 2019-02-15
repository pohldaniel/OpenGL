#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_normalMatrix;

uniform mat4 mvp;

out vec2 v_texCoord;
out vec3 v_normal;

out vec4 vPosLS;

void main(void){
	
   vPosLS = u_projection  * u_modelView * vec4(i_position, 1.0);
  vPosLS /= vPosLS.wwww;
  
  gl_Position =  vPosLS;
	
   v_normal = normalize((u_normalMatrix * vec4(i_normal, 0.0)).xyz);
   
    
  
   v_texCoord = i_texCoord;
}