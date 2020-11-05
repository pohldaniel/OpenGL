#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_color;
layout(location = 3) in vec3 i_normal;
layout(location = 4) in vec3 i_tangent;
layout(location = 5) in vec3 i_bitangent;

uniform mat4 u_projection;
uniform mat4 u_modelViewLight;
uniform mat4 u_modelView;
uniform mat4 u_model;
uniform mat4 u_normalMatrix;

uniform vec3 u_lightPos[2];

out vec2 v_texCoord;
out vec3 eyeSpaceVert;
out vec3 eyeSpaceTangent;
out vec3 eyeSpaceBinormal;
out vec3 eyeSpaceNormal;
out vec3 eyeSpaceLight[2];



void main(void){

	eyeSpaceVert = (u_modelView * vec4(i_position, 1.0)).xyz;
	
	gl_Position =   u_projection * vec4(eyeSpaceVert, 1.0);
	
	
	eyeSpaceTangent  = normalize((u_normalMatrix * vec4(i_tangent, 0.0)).xyz);
	eyeSpaceBinormal = normalize((u_normalMatrix * vec4(i_bitangent, 0.0)).xyz);
	eyeSpaceNormal   = normalize((u_normalMatrix * vec4(i_normal, 0.0)).xyz);
		
	v_texCoord = i_texCoord;

	
	
	
	for(int i = 0; i < 2; i++){
	
		eyeSpaceLight[i] =  (u_modelViewLight * vec4(u_lightPos[i], 1.0)).xyz;
	 
	}		
}