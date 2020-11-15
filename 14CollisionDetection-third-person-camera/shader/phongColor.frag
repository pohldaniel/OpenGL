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
in vec3 v_lightDirection[2];
in vec3 v_viewDirection;


out vec4 color;


uniform LightSource light[2];
uniform Material material;

uniform float active[2];

void main(void){

	int count = 0;
	
	vec3 E = normalize(v_viewDirection);
	vec3 N = normalize(v_normal);
	
	vec3 ambient  = vec3(0.0, 0.0, 0.0);
	vec3 diffuse  = vec3(0.0, 0.0, 0.0);
	vec3 specular = vec3(0.0, 0.0, 0.0);
	
	for(int i = 0; i < 2; i++){
		if(active[i] > 0.0){
			ambient += light[i].ambient;
			
			float lambert = dot(normalize(v_lightDirection[i]),N );
			
			if(lambert > 0.0){
			
				diffuse += normalize(light[i].diffuse) * lambert;
				
				vec3 V = reflect(normalize(-v_lightDirection[i]),N);
				specular += pow( max( dot(E,V),0.0 ), material.specularShininesse );
			}
		}else{
		
			count++;
		}
	}
	
	
	
	ambient *= material.ambient;
	diffuse *= material.diffuse;
	specular *= material.specular;
	
	if(count == 2){
		color = vec4(0.0, 0.0, 0.4, 1.0);
	
	}else{
		color = vec4(ambient + diffuse + specular, 1.0);
	}
}












