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
uniform mat4 u_normalMatrix;

uniform vec3 u_lightPos[2];


out vec3 v_lightDirection[2];
out vec3 v_viewDirection;
out vec2 v_texCoord;


vec3 vertexLight[2];

void main(void){

	vec3 normal = normalize((u_normalMatrix * vec4(i_normal, 0.0)).xyz);
	vec3 tangent = normalize((u_normalMatrix * vec4(i_tangent.xyz, 0.0)).xyz);
	vec3 bitangent = normalize((u_normalMatrix * vec4(i_bitangent, 0.0)).xyz);
	
	mat3 TBN = mat3(tangent.x, bitangent.x, normal.x,
					tangent.y, bitangent.y, normal.y,
					tangent.z, bitangent.z, normal.z);
	
	vec3 vertexInCamSpace = (u_modelView * vec4(i_position, 1.0)).xyz;
	
	
	v_viewDirection = - TBN * vertexInCamSpace;
	
	for(int i = 0; i < 2; i++){
	
		v_lightDirection[i] =  TBN *((u_modelViewLight * vec4(u_lightPos[i], 1.0)).xyz - vertexInCamSpace);
	 
	}
	
	 
	gl_Position =   u_projection * u_modelView * vec4(i_position, 1.0);
	v_texCoord = i_texCoord;
	
}