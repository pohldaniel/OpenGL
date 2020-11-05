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
in vec2 v_texCoord;

out vec4 color;


uniform LightSource light[2];
uniform Material material;
uniform float active[2];

uniform sampler2D u_textureColor;

void main(void){

	int count = 0;
	vec3 diffuseColor = texture2D(u_textureColor, v_texCoord ).rgb;
	vec3 E = normalize(v_viewDirection);
	vec3 N = normalize(v_normal);
	
	vec3 ambient  = vec3(0.0, 0.0, 0.0);
	vec3 diffuse  = vec3(0.0, 0.0, 0.0);
	vec3 specular = vec3(0.0, 0.0, 0.0);
	
	for(int i = 0; i < 2; i++){
		if(active[i] > 0.0){
			ambient += light[i].ambient;
			
			float lambert = dot(normalize(v_lightDirection[i]),N );
			float level = floor(lambert * 3.0 );
			lambert = level / 3.0;
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
		color =  vec4( diffuseColor * (ambient + diffuse + specular) , 1.0);	
	}
	
	
}












