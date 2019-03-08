#version 410 core

vec4 u_diffuseColor = vec4(0.2, 0.8, 0.5, 1.0);
vec4 u_scatterColor = vec4(0.8, 0.8, 0.8, 1.0);


uniform float u_wrap = 0.1;
uniform float u_scatterWidth = 0.5;
uniform float u_scatterFalloff = 0.05;
uniform float near = 1.0; 
uniform float far  = 100.0; 

uniform sampler2D u_texture;
uniform sampler2D u_texture2;


uniform mat4 u_view;
uniform mat4 u_model;

uniform vec3 lightPos;


in vec3 v_normal;
in vec4 v_peye;
in vec4 sc;
in vec4 sc2;

out vec4 color;


float wrapLighting(float x, float wrap){

	return (x + wrap) / (1.0 + wrap);
}

float scatterTint(float x, float scatterWidth){

	if (scatterWidth == 0.0)
	{
		return 0.0;
	}

	return smoothstep(0.0, scatterWidth, x) * smoothstep(scatterWidth * 2.0, scatterWidth, x); 
}

float getDepthPassSpaceZ(float zWC){

	// Assume standard depth range [0..1]
	float zNDC = (zWC - 0.5) * 2.0;

	return 2*far*near / (zNDC*(near-far)+far+near);
}


void main(void){

   vec3 fragment = vec3(v_peye);
   vec3 normal   = normalize(v_normal);
   vec3 viewDir  = normalize(-fragment);

   vec3 light = vec3(u_view * vec4(lightPos, 1.0));
   
   vec3 positionToLight = light - fragment;
   vec3 lightDir  = normalize(positionToLight);
   
   vec4 scPostW = sc/sc.w;
    
	
	
   float zIn =  textureProj(u_texture, sc ).z;
   float zOut = getDepthPassSpaceZ(sc.z/sc.w);
   
   
  
   
   float scatterFalloff = exp(-abs(zIn - zOut) * u_scatterFalloff);
   
   float NdotL = dot(normal, lightDir);		
   float NdotL_wrap = wrapLighting(NdotL, u_wrap);
   
   float diffuseIntensity = max(NdotL, 0.0);		
   float scatterIntensity = scatterTint(NdotL_wrap, u_scatterWidth);
   
 if ( sc.w <= 0.0f || (scPostW.x < 0 || scPostW.y < 0) || (scPostW.x >= 1 || scPostW.y >= 1)  ){
   
    color = vec4(1.0, 1.0, 1.0, 1.0)  ;
	
   }else{
   
   //color = vec4(1.0, 1.0, 1.0, 1.0) - (vec4(1.0, 1.0, 1.0, 1.0) - textureProj(u_texture, sc )) * diffuseIntensity;
   
	color = textureProj(u_texture, sc );
   }
   
  
    color = u_diffuseColor * diffuseIntensity + u_scatterColor * scatterIntensity * scatterFalloff;
}
