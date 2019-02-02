#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_color;
layout(location = 3) in vec3 i_normal;
layout(location = 4) in vec3 i_tangent;
layout(location = 5) in vec3 i_bitangent;

uniform mat4 u_projection;


uniform mat4 u_viewMatrix;
uniform mat4 u_modelMatrix;
uniform mat4 u_normalMatrix;

uniform mat4 u_depthPassMatrix;

uniform vec3 u_lightPos;
uniform mat4 u_modelLight;

out vec3 v_normal;
out vec4 v_projCoord;
out vec3 v_lightdirection;

void main(void){

	v_lightdirection = (u_projection * u_viewMatrix *(vec4(i_position, 1.0) - u_modelLight * vec4(u_lightPos, 1.0))).xyz;

	vec4 vertex = u_modelMatrix * vec4(i_position, 1.0);

	// Bring the vertex to the light / depth pass coordinate system.
	v_projCoord = u_depthPassMatrix * vertex;

	vertex = u_viewMatrix * vertex;

	v_normal = (u_normalMatrix * vec4(i_normal, 0.0)).xyz;

	gl_Position = u_projection*vertex;
	
}