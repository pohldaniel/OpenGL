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

uniform sampler2D u_textureColor;
uniform sampler2D u_textureNormal;
uniform sampler2D u_displace;

uniform float u_heighScale;
uniform float u_tile;
uniform bool u_shadow = false;

in vec2 v_texCoord;
in vec3 eyeSpaceVert;
in vec3 eyeSpaceTangent;
in vec3 eyeSpaceBinormal;
in vec3 eyeSpaceNormal;
in vec3 eyeSpaceLight[2];



out vec4 color;


float ray_intersect_rm( vec2 dp, vec2 ds );
vec4 relief_map();
vec4 relief_map2();
vec4 relief_map_shadow();

void main(void){

	if(u_shadow){
	
		color = relief_map_shadow();
	}else{
		color = relief_map2();
	}
}


float ray_intersect_rm( vec2 dp, vec2 ds ){

	const int linearSearchSteps = 40;
	const int binarySearchSteps = 20;
 
	float depthStep = 1.0 / float( linearSearchSteps );
 
	// current size of search window
	float size = depthStep;
 
	// current depth position
	float depth = 0.0;
 
	// best match found (starts with last position 1.0)
	float bestDepth = 1.0;
 
	// search front to back for first point inside object
	for( int i = 0; i < linearSearchSteps - 1; ++i )
	{
		depth += size;
 
		float t = texture2D( u_displace, dp + ds * depth ).r;
 
		if( bestDepth > 0.996 )		// if no depth found yet
			if( depth >= t )
				bestDepth = depth;	// store best depth
	}
 
	depth = bestDepth;
 
	// recurse around first point (depth) for closest match
	for( int i = 0; i < binarySearchSteps; ++i )
	{
		size *= 0.5;
 
		float t = texture2D( u_displace, dp + ds * depth ).r;
 
		if( depth >= t )
		{
			bestDepth = depth;
			depth -= 2.0 * size;
		}
 
		depth += size;
	}
 
	return bestDepth;
}

vec4 relief_map(){

	vec3 normal;
	vec3 p,v,l,s,c;
	vec2 dp,ds,uv;
	float d;
 
 
	// Ray intersect in view direction
	p  = eyeSpaceVert; 
	v  = normalize( p );
	v = normalize( vec3( dot( v, eyeSpaceTangent ), dot( v, eyeSpaceBinormal ), dot( v, eyeSpaceNormal ) ) );
	s  = v;
 
	s  *= u_heighScale / -s.z;
	dp = v_texCoord * u_tile;
	ds = s.xy;
	
	d  = ray_intersect_rm( dp, ds );
 
	// get rm and color texture points
	uv = dp + ds * d;

	normal = texture2D( u_textureNormal, uv ).rgb;
	c  = texture2D( u_textureColor, uv ).rgb;
 
	// expand normal from normal map in local polygon space
	normal.xy  = normal.xy * 2.0 - 1.0;
	normal.z   = sqrt( 1.0 - dot( normal.xy, normal.xy ) );
	normal.xyz = normalize( normal.x * eyeSpaceTangent - normal.y * eyeSpaceBinormal + normal.z * eyeSpaceNormal );
	
	// position eyeSpace
	p += s * d;
	
	int count = 0;
	
	vec3 ambient  = vec3(0.0, 0.0, 0.0);
	vec3 diffuse  = vec3(0.0, 0.0, 0.0);
	vec3 specular = vec3(0.0, 0.0, 0.0);
	
	for(int i = 0; i < 2; i++){
		if(active[i] > 0.0){
		
			ambient += light[i].ambient;
			
			l = normalize(p - eyeSpaceLight[i]);
		
			float lambert = max( dot(l , normal), 0.0);
			
			if(lambert > 0.0){
				
				diffuse += light[i].diffuse * lambert;
						
				vec3 reflected = reflect(-l, normal);
				specular += light[i].specular * pow( max( dot(-v,reflected),0.0 ), material.specularShininesse ) * 4;		
			}
			
		}else{
		
			count++;
		}
	}
	
	ambient *= material.ambient;
	diffuse *= material.diffuse;
	specular *= material.specular;
	
	if(count == 2){
	
	return	color = vec4(0.0, 0.0, 0.0, 1.0);
	
	}else{
	
	return	color =  vec4( c* ( ambient + diffuse ) + specular, 1.0);
	
	}

}

vec4 relief_map2(){

	vec3 normal, diffuseColor, tangentSpacelight;
	vec3 p,v,l,s;
	vec2 dp,ds,uv;
	float d;
 
 
	// Ray intersect in view direction
	p  = eyeSpaceVert; 
	v  = normalize( p );
	//v = normalize( vec3( dot( v, eyeSpaceTangent ), dot( v, eyeSpaceBinormal ), dot( v, eyeSpaceNormal ) ) );
	s  = normalize( vec3( dot( v, eyeSpaceTangent ), dot( v, eyeSpaceBinormal ), dot( v, eyeSpaceNormal ) ) );
 
	s  *= u_heighScale / -s.z;
	dp = v_texCoord * u_tile;
	ds = s.xy;
	
	d  = ray_intersect_rm( dp, ds );
 
	// get rm and color texture points
	uv = dp + ds * d;

	normal  = normalize(texture2D( u_textureNormal, uv ).rgb * 2.0 - 1.0) ;
	diffuseColor = texture2D( u_textureColor, uv ).rgb;
 
	// position eyeSpace
	p += s * d;
	
	int count = 0;
	
	vec3 ambient  = vec3(0.0, 0.0, 0.0);
	vec3 diffuse  = vec3(0.0, 0.0, 0.0);
	vec3 specular = vec3(0.0, 0.0, 0.0);
	
	for(int i = 0; i < 2; i++){
		if(active[i] > 0.0){
		
			l = normalize( eyeSpaceLight[i] - p);
			tangentSpacelight  = normalize( vec3( dot( l, eyeSpaceTangent ), dot( l, eyeSpaceBinormal ), dot( l, eyeSpaceNormal )));
			
			ambient += light[i].ambient;
			
			float lambert = max(dot( tangentSpacelight, normal ), 0.0 );	
			
			if( lambert > 0.0){
				diffuse += light[i].diffuse * lambert;
				
				vec3 reflected = reflect(-tangentSpacelight, normal);
				specular += light[i].specular * pow( max( dot(-v,reflected),0.0 ), material.specularShininesse ) * 4;
			}
							
		}else{
		
			count++;
		}
	}
	
	ambient *= material.ambient;
	diffuse *= material.diffuse;
	specular *= material.specular;
	
	if(count == 2){
	
		return vec4(0.0, 0.0, 0.0, 1.0);
	
	}else{
	
		return vec4( diffuseColor * ( ambient + diffuse ) + specular, 1.0);
	}
}

vec4 relief_map_shadow(){

	vec3 normal, diffuseColor, tempSpecular;
	vec3 p,v,l,s;
	vec2 dp,ds,uv;
	float d, dl;
 
 
	// Ray intersect in view direction
	p  = eyeSpaceVert; 
	v  = normalize( p );
	//v = normalize( vec3( dot( v, eyeSpaceTangent ), dot( v, eyeSpaceBinormal ), dot( v, eyeSpaceNormal ) ) );
	s  = normalize( vec3( dot( v, eyeSpaceTangent ), dot( v, eyeSpaceBinormal ), dot( v, eyeSpaceNormal ) ) );
 
	s  *= u_heighScale / -s.z;
	dp = v_texCoord * u_tile;
	ds = s.xy;
	
	d  = ray_intersect_rm( dp, ds );
 
	// get rm and color texture points
	uv = dp + ds * d;

	normal  = normalize(texture2D( u_textureNormal, uv ).rgb * 2.0 - 1.0) ;
	diffuseColor = texture2D( u_textureColor, uv ).rgb;
	
	// position eyeSpace
	p += s * d;
	
	int count = 0;
	
	vec3 ambient  = vec3(0.0, 0.0, 0.0);
	vec3 diffuse  = vec3(0.0, 0.0, 0.0);
	vec3 specular = vec3(0.0, 0.0, 0.0);
	
	for(int i = 0; i < 2; i++){
		if(active[i] > 0.0){
		
			tempSpecular = light[i].specular;
			ambient += light[i].ambient;
			
			l = normalize(p - eyeSpaceLight[i]);
		
			// ray intersect in light direction
			dp += ds * d;
			s  = normalize( vec3( dot( l, eyeSpaceTangent ), dot( l, eyeSpaceBinormal ), dot( l, eyeSpaceNormal ) ) );
			
			vec3 tangentSpacelight = s;
			
			s  *= u_heighScale / dot( eyeSpaceNormal, -l );
			dp -= d * s.xy;
			ds  = s.xy;
 
			dl = ray_intersect_rm( dp, s.xy );
					
			// if pixel in shadow
			if( dl < (d - 0.05) ) {
			
				diffuseColor *= 0.8;		
				tempSpecular = vec3(0.0);
			} 
		
		
			float diff = max(dot( -tangentSpacelight, normal ), 0.0 );	
			diffuse += light[i].diffuse * diff;
			
			vec3 reflected = reflect(tangentSpacelight, normal);
			specular += tempSpecular * pow( max( dot(-v,reflected),0.0 ), material.specularShininesse ) * 4;
							
		}else{
		
			count++;
		}
	}
	
	ambient *= material.ambient;
	diffuse *= material.diffuse;
	specular *= material.specular;
	
	if(count == 2){
	
		return vec4(0.0, 0.0, 0.0, 1.0);
	
	}else{
	
		return vec4( diffuseColor * ( ambient + diffuse ) + specular, 1.0);
	}
	

}