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

uniform LightSource light[2];
uniform Material material;
uniform float active[2];

uniform sampler2D u_texture;
uniform sampler2D u_textureNormal;


in vec3 v_lightDirection[2];
in vec3 v_viewDirection;
in vec2 v_texCoord;

out vec4 color;




void main(void){

	int count = 0;
	vec3 diffuseColor = texture2D( u_texture, v_texCoord ).rgb;
	
	vec3 E = normalize(v_viewDirection);
	vec3 N = normalize(texture2D( u_textureNormal, v_texCoord ).rgb * 2.0 - 1.0) ;
	
	vec3 ambient  = vec3(0.0, 0.0, 0.0);
	vec3 diffuse  = vec3(0.0, 0.0, 0.0);
	vec3 specular = vec3(0.0, 0.0, 0.0);
	
	for(int i = 0; i < 2; i++){
		if(active[i] > 0.0){
			ambient += light[i].ambient;
			
			float lambert = max(dot(normalize(v_lightDirection[i]),N ), 0.0);
			
			if( lambert > 0.0){
			
				diffuse += light[i].diffuse * lambert;
				
				vec3 V = reflect(normalize(-v_lightDirection[i]),N);
				specular += light[i].specular * pow( max( dot(E,V),0.0 ), 2.0 );
			}
		}else{
		
			count++;
		}
	}
	
	ambient *= material.ambient;
	diffuse *= material.diffuse;
	specular *= material.specular;
	
	if(count == 2){
		color = vec4(0.0, 0.0, 0.0, 1.0);
	
	}else{
		color =  vec4( diffuseColor* (ambient + diffuse ) + specular, 1.0);
	}
	
}












