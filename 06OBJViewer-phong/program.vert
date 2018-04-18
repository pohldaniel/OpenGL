#version 410 core

#define PI 3.1415926535897932384626433832795


struct Light{

	vec3 position;
	vec3 color;
};

mat4 rotate(vec3 axis, float degrees){
	
	float rad = (degrees*PI)/180.0;
	float x	  = axis.x;
	float y	  = axis.y;
	float z	  = axis.z;
	float c	  = cos(rad);
	float s   = sin(rad);
	
	return mat4(
	(x*x)*(1.0-c) + c    , (x*y)*(1.0-c) - (z*s) , (z*x)*(1.0-c) + (y*s) , 0.0 ,
	(x*y)*(1.0-c) + (z*s), (y*y)*(1.0-c) + c	 , (z*y)*(1.0-c) - (x*s) , 0.0 ,
	(x*z)*(1.0-c) - (y*s), (y*z)*(1.0-c) + (x*s) , (z*z)*(1.0-c) + c     , 0.0 ,
	 0.0				 ,  0.0				 	 ,  0.0					 , 1.0
	);
}

mat4 shear(float hxy, float hxz, float hyx, float hyz, float hzx, float hzy){

	float D = 1- (hxy*hyx) - (hxz*hzx) - (hyz*hzy) + (hxy*hyz*hzx) + (hxz*hyx*hzy);

	return mat4(
		1-(hyz*hzy)      , -hyx + (hyz*hzx) , -hzx + (hyx*hzy)   , 0.0 ,
		-hxy + (hxz*hzy) , 1 - (hxz*hzx)  	, -hzy + (hxy*hzx)   , 0.0 ,
		-hxz + (hxy*hyz) , -hyz + (hxz*hyx) , 1 - (hxy*hyx)    	 , 0.0 ,
		0.0		     	 ,  0.0			    , 0.0			     , D
	)*1.0/D;

}


mat4 translate(float dx, float dy, float dz){

	return mat4(
	1.0 , 0.0 , 0.0 , 0.0 ,
	0.0 , 1.0 , 0.0 , 0.0 ,
	0.0 , 0.0 , 1.0 , 0.0 ,
	dx , dy , dz , 1.0
	);
}

mat4 scale(float a, float b, float c){

	if(a == 0) a = 1.0;
	if(b == 0) b = 1.0;
	if(c == 0) c = 1.0;
	
	return mat4(
	1.0/a , 0.0   , 0.0   , 0.0 ,
	0.0   , 1.0/b , 0.0   , 0.0 ,
	0.0   , 0.0   , 1.0/c , 0.0 ,
	0.0   , 0.0   , 0.0   , 1.0
	);
}

float calcDiffuse(vec3 pos, vec3 normal, vec3 lightPos){

	vec3 L = normalize(lightPos - pos);
	float diff = dot(L, normal);
	
	if(diff > 0){
	
		return diff;
		
	}else{
	
		return 0.0;
	}
	
}

float calcSpecular (vec3 pos, vec3 normal, vec3 viewDir, int n, vec3 lightPos){

	vec3 L = normalize(lightPos - pos);
	vec3 V = (L - normal * 2.0 * dot(L, normal));
	
	float spec = dot(viewDir, V);
	
	if(spec > 0.0){
	
		return pow (spec, n);
	
	}else{
	
		return 0.0;
	}
}

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 color;
layout(location = 3) in vec3 a_normal;

uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_normalMatrix;
uniform vec3 u_color;




out vec2 v_texCoord;
out vec3 v_color;
out vec3 v_normal;
out vec3 v_halfway;

const vec3 light = normalize (vec3 (2, 1, 3));

void main(void){

	gl_Position =   u_projection * u_modelView * vec4(position, 1.0);
	v_normal = normalize((u_normalMatrix * vec4(a_normal, 0.0)).xyz);
	vec3 view    = normalize (- vec3 (u_modelView * vec4(position, 1.0)));
	v_halfway      = normalize (view + light);
	
}