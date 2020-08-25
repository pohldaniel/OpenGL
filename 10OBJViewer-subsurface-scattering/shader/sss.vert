#version 410 core

uniform vec3 u_lightPos; // world space

uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_normalMatrix;

uniform mat4 u_projectionShadowBias;
uniform mat4 u_projectionShadow;
uniform mat4 u_projectionShadowD3D;
uniform mat4 u_viewShadow;

attribute vec4 position;
attribute vec3 normal;

layout(location = 0) in vec3 i_position;
layout(location = 3) in vec3 i_normal;

varying vec3 frag_eye_pos;
varying vec3 frag_eye_dir;
varying vec3 frag_eye_normal;
varying vec3 frag_eye_light_normal;
varying vec3 frag_eye_light_pos;

varying vec4 sc;

void main(){    

    frag_eye_pos = (u_modelView * vec4(i_position, 1.0)).xyz;
    frag_eye_dir = normalize(-frag_eye_pos);
    frag_eye_normal = normalize((u_normalMatrix * vec4(i_normal, 0.0)).xyz); 
    
    frag_eye_light_pos = (u_view * vec4(u_lightPos, 1.0)).xyz; // <-- assumes view matrix == identity matrix   //(modelview * light_pos).xyz;
    frag_eye_light_normal = normalize(frag_eye_light_pos - frag_eye_pos);
    
    gl_Position = u_projection *  u_modelView  * vec4(i_position, 1.0);
	
	vec3 shrinkedPos = i_position - 0.005 * normalize(i_normal);
	
	//sc = u_projectionShadowBias * u_viewShadow * u_model * vec4(shrinkedPos, 1.0);
	
	sc = u_projectionShadow * u_viewShadow * u_model * vec4(shrinkedPos, 1.0);
    sc.xyz = sc.xyz * 0.5 + vec3(0.5)*sc.w;
	
	//sc = u_projectionShadowD3D * u_viewShadow * u_model * vec4(shrinkedPos, 1.0);
    //sc.xy = sc.xy * 0.5 + vec2(0.5)*sc.w;

	//use m_projMatrixD3D for rendering the shadowmap
    //sc.xyz = sc.xyz * 0.5 + vec3(0.5)*sc.w;
}
