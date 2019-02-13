#version 410 core



vec4 u_diffuseColor = vec4(0.8, 0.0, 0.0, 1.0);
vec4 u_scatterColor = vec4(0.8, 0.8, 0.0, 1.0);

vec2 u_nearFar = vec2(1.0, 100.0);
float u_wrap = 0.1;
float u_scatterWidth = 0.5;
float u_scatterFalloff = 5.0;

uniform sampler2D u_texture;
uniform mat4 u_view;
uniform vec3 lightPos;


in vec3 v_normal;
in vec4 v_peye;
in vec4 sc;

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

	float n = u_nearFar.x;
	float f = u_nearFar.y;
	
	// Assume standard depth range [0..1]
	float zNDC = (zWC - 0.5) * 2.0;

	return -2*f*n / (zNDC*(n-f)+f+n);
}


void main(void){

   vec3 fragment = vec3(v_peye);
   vec3 normal   = normalize(v_normal);
   vec3 viewDir  = normalize(-fragment);

   vec3 light = vec3(u_view * vec4(lightPos, 1.0));
   
   vec3 positionToLight = light - fragment;
   vec3 lightDir  = normalize(positionToLight);
   
   vec4 scPostW = sc / sc.w;
   
   float zInWC = texture2D(u_texture, scPostW.xy).z;
   float zOutWC = sc.z / sc.w;
	
   float zIn = getDepthPassSpaceZ(zInWC); 
   float zOut = getDepthPassSpaceZ(zOutWC);
   
   
   float scatterFalloff = exp(-abs(zOut - zIn) * u_scatterFalloff);
   
   float NdotL = dot(normal, lightDir);		
   float NdotL_wrap = wrapLighting(NdotL, u_wrap);
   
   float diffuseIntensity = max(NdotL, 0.0);		
   float scatterIntensity = scatterTint(NdotL_wrap, u_scatterWidth);
   
   color = u_diffuseColor * diffuseIntensity + u_scatterColor * scatterIntensity * scatterFalloff;
   //color = u_scatterColor * scatterIntensity * scatterFalloff;
   
	
}
