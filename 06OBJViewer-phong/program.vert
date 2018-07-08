#version 410 core


layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 color;
layout(location = 3) in vec3 normal;

uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_normalMatrix;

uniform vec3 u_lightPos;

out vec3 v_normal;
out vec3 v_lightDirection;
out vec3 v_viewDirection;


void main(void){

	
	mat4 normalMatrix =transpose(inverse(u_modelView));
	v_normal = (u_normalMatrix * vec4(normal, 0.0)).xyz;
	
	vec3 vertexInCamSpace = (u_modelView * vec4(position, 1.0)).xyz;
	v_viewDirection = -vertexInCamSpace;
	
	vec3 lightInCamSpace = (u_modelView * vec4(u_lightPos, 1.0)).xyz;
	v_lightDirection = lightInCamSpace - vertexInCamSpace;
	
	gl_Position =   u_projection * u_modelView * vec4(position, 1.0);
	
}