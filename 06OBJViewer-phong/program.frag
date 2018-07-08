#version 410 core

struct LightSource{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
};

struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float specularShininesse;
};


in vec3 v_normal;
in vec3 v_lightDirection;
in vec3 v_viewDirection;


out vec4 color;


uniform LightSource light;
uniform Material material;



void main(void){

	
	vec3 E = normalize(v_viewDirection);
	vec3 N = normalize(v_normal);
	vec3 L = normalize(v_lightDirection);
	
	//phong 
	vec3 V = N * 2.0 * dot(N, L) - L;
	
	//blinn phong
	vec3 H = normalize(E + L);
	
	vec3 ambient  = vec3(0.0, 0.0, 0.0);
	vec3 diffuse  = vec3(0.0, 0.0, 0.0);
	vec3 specular = vec3(0.0, 0.0, 0.0);
	
	ambient += light.ambient;
	diffuse += light.diffuse * max(dot(L,N),0.0);
	
	//phong
	specular += light.specular * pow(max(dot(E,V),0.0),material.specularShininesse);
	
	//blinn phong
	//specular += light.specular * pow(max(dot(H,N),0.0),material.specularShininesse);
	
	ambient *= material.ambient;
	diffuse *= material.diffuse;
	specular *= material.specular;
	
	color = vec4(ambient + diffuse + specular, 1.0);
	
}












