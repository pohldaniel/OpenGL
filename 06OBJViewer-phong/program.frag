#version 410 core

uniform sampler2D u_texture;
uniform bool u_hasTexture;

const vec4 Ca = vec4 ( 0,  0, .3, 0);
const vec4 Ce = vec4 ( 0,  0,  0, 0);
const vec4 Cd = vec4 ( 0, .5,  0, 0);
const vec4 Cs = vec4 (.8, .8, .8, 0);
const float kse = 30;

// Direktionales Licht (im Unendlichen)
const vec3 light = normalize (vec3 (2, 1, 3));	

in vec3 v_color;
in vec3 v_normal;
in vec3 v_halfway;
in vec2 v_texCoord;

out vec4 color;

void main(void){

	
	
	 vec3  n  = normalize (v_normal);
    vec3  h  = normalize (v_halfway);
    float Id = max (dot (light, n), 0);
    float Is = pow (max (dot (h, n), 0), kse);
    color = Ca + Ce + Cd * Id + Cs * Is;
	
	color = Ca + Ce + Cd * Id + Cs * Is;
	
	
	
}
