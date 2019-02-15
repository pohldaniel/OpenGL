#version 410 core

uniform sampler2D u_texture;
uniform float refr_index = 0.5;
uniform float irradiance_intensity = 1.5;

in vec2 v_texCoord;
in vec3 v_normal;
in vec4 v_posLS;
in vec3 v_lightDirection;

out vec4 color;

void main(void){


	 vec3 N = normalize(v_normal);
	 vec3 V = normalize(v_lightDirection);
	 
	 float k = pow(1.0 - max( dot(N,V),0.0 ), 5.0);
     float Ft = 1.0 + k *(refr_index - 1.0);
	 vec3 E = vec3(Ft * max(0.0, dot(N,V) )  ) * irradiance_intensity;
	 
	 vec3 color = E * texture2D( u_texture, v_texCoord ).rgb;
	 
	 gl_FragColor = vec4(color, -sign(abs(N.x)));
	 
	
}
