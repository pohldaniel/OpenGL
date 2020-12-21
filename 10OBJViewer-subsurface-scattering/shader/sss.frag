#version 410 core

#define CONSTANT_ATTENUATION 1.0
#define LINEAR_ATTENUATION 0.7
#define QUADRATIC_ATTENUATION 1.8

uniform float near = 1.0; 
uniform float far  = 100.0; 
uniform float u_thicknessScale = 0.5;
uniform float u_sssScale = 0.2;

uniform sampler2D u_texture;

uniform vec4 mat_diffuse = vec4(0.0, 0.45, 0.75, 1.0);
uniform vec4  light_diffuse = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec3  ambient_color= vec3(0.04, 0.04, 0.04);
uniform vec3  specular = vec3(0.0, 0.0, 0.0);
uniform float shininess = 200.0;
uniform float ambient = 2.0;
uniform int   power = 1;
uniform float distortion = 0.4;
uniform float scale = 8.0;
uniform float light_radius = 40.0;


varying vec3 frag_eye_pos;
varying vec3 frag_eye_dir;
varying vec3 frag_eye_normal;
varying vec3 frag_eye_light_normal;
varying vec3 frag_eye_light_pos;

varying vec4 scOrth;
varying vec4 scPers;

float saturate(float val) {
    return clamp(val, 0.0, 1.0);
}

vec3 phong_shade(vec3 light_pos, vec3 eye, vec3 normal, float attentuation) {
    vec3 L = normalize(frag_eye_light_normal);  
    vec3 N = normalize(normal);
    vec3 R = reflect(-L, N);
    vec3 E = normalize(-eye);
    
    float ld = max(dot(N, L), 0.0);
    float ls = pow(max(dot(R, E), 0.0), 0.3 * shininess);
    return (ambient_color + (ld * mat_diffuse.xyz) + (ls * specular)) * attentuation;
}

vec3 blinn_shade(vec3 light_pos, vec3 eye, vec3 normal, float attentuation) {
    vec3 L = normalize(light_pos - eye);  
    vec3 N = normalize(normal);
    vec3 V = -normalize(eye);
    float dot2 = max(dot(N, L), 0.0);
    vec3 c = mat_diffuse.xyz * light_diffuse.xyz * dot2;
    vec3 H = normalize(V + L);
    vec3 specular = vec3(clamp(6.0 * pow(max(dot(N, H), 0.0), shininess), 0.0, 1.0));
    return (c + specular) * attentuation;
	
	//return c + specular;
}

vec3 blinn_shade2(vec3 light_pos, vec3 eye, vec3 normal, float attentuation) {

	vec4 diffuse = vec4(0.0, 0.75, 0.45, 1.0);   

    vec3 L = normalize(light_pos - eye);  
    vec3 N = normalize(normal);
    vec3 V = -normalize(eye);
    float dot2 = max(dot(N, L), 0.0);
    vec3 c = diffuse.xyz * light_diffuse.xyz * dot2;
    vec3 H = normalize(V + L);
    vec3 specular = vec3(clamp(6.0 * pow(max(dot(N, H), 0.0), 2000), 0.0, 1.0));
    return (c + specular) * attentuation;
	
	//return c + specular;
}


vec4 sss(float thickness, float attentuation) {
    vec3 light = normalize(frag_eye_light_normal + (frag_eye_normal * distortion));
    float dot2 = pow(saturate(dot(frag_eye_dir, -light)), power) * scale;
    float lt = attentuation * (dot2 + ambient) * thickness;
    //lt = 1.0 - lt;
	
    return clamp((mat_diffuse * light_diffuse * lt), 0.0,1.0);
}

float getDepthPassSpaceZ(float zWC, float near, float far){

	// Assume standard opengl depth range [0..1]
    float z_n = 2.0 * zWC - 1.0;
    float z_e = (2.0 * near * far) / (far + near + z_n * (near - far));	//[near, far]

	//divided by far to get the range [near/far, 1.0] just for visualisation
	//float z_e = (2.0 * near) / (far + near + z_n * (near - far));	

	return z_e;
}

float getDepthPassSpaceZD3D(float zWC, float near, float far){

    float z_e = (near * far) / (far + zWC * (near - far));	//[near, far]

	//divided by far to get the range [near/far, 1.0] just for visualisation
	//float z_e = near / (far + zWC * (near - far));	

	return z_e;
}

void main(){

   //vec4 scPostW = (sc/scPers.w);
   //float zIn =  texture2D(u_texture, scPers.xy).r;
   float zInPers =  textureProj(u_texture, scPers).r;
   float zOutPers = scPers.z /scPers.w;

   zInPers = getDepthPassSpaceZ(zInPers, 1.0, 100.0);
   zOutPers = getDepthPassSpaceZ(zOutPers, 1.0, 100.0);

   //zIn = getDepthPassSpaceZD3D(zIn, 1.0, 100.0);
   //zOut = getDepthPassSpaceZD3D(zOut, 1.0, 100.0);
	
   float zInOrth =  texture2D(u_texture, scOrth.xy).r;
   float zOutOrth = scOrth.z;
	
   float thickness = (zOutOrth - zInOrth) * u_thicknessScale;
   //float thickness = (zOutPers - zInPers) * u_thicknessScale;
   
   thickness = (1.0 - thickness) * u_sssScale;
        
   vec3 light_dir = (frag_eye_light_pos - frag_eye_pos) / light_radius;
   float light_attentuation = max(1.0 - dot(light_dir, light_dir), 0.0); //1.0 / (CONSTANT_ATTENUATION + LINEAR_ATTENUATION * d + QUADRATIC_ATTENUATION * d * d);
   //light_attentuation = 0.6;
   //vec3 phong_color = phong_shade(frag_eye_light_pos, frag_eye_pos, frag_eye_normal, light_attentuation);
   vec3 blinn_color = blinn_shade(frag_eye_light_pos, frag_eye_pos, frag_eye_normal, light_attentuation);
   gl_FragColor = (vec4(blinn_color, 1.0)) + sss(thickness, light_attentuation);

   //gl_FragColor = vec4(thickness);
}
