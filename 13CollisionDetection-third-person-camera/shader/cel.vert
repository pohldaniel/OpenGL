#version 410 core





layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 3) in vec3 i_normal;

uniform mat4 u_projection;
uniform mat4 u_modelViewLight;
uniform mat4 u_modelView;
uniform mat4 u_normalMatrix;

uniform vec3 u_lightPos[2];



out vec3 v_normal;
out vec3 v_lightDirection[2];
out vec3 v_viewDirection;
out vec2 v_texCoord;

vec3 vertexLight[2];

void main(void){

	
	
	
	//v_normal = (transpose(inverse(u_modelView)) * vec4(i_normal, 0.0)).xyz;
	v_normal = normalize((u_normalMatrix * vec4(i_normal, 0.0)).xyz);
	
	//v_normal = i_normal;
	
	
	vec3 vertexInCamSpace = (u_modelView * vec4(i_position, 1.0)).xyz;
	v_viewDirection = -vertexInCamSpace;
	
	for(int i = 0; i < 2; i++){
	 vertexLight[i] =  (u_modelViewLight * vec4(u_lightPos[i], 1.0)).xyz - vertexInCamSpace;
	}
	
	v_lightDirection = vertexLight;
	
	//vec3 lightInCamSpace = (u_modelView * vec4(u_lightPos, 1.0)).xyz;
	//v_lightDirection = lightInCamSpace - vertexInCamSpace;
	
	gl_Position =   u_projection * u_modelView * vec4(i_position, 1.0);
	
	
	v_texCoord = i_texCoord;
}