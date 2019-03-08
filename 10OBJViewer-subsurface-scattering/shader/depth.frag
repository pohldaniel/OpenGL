#version 410 core

out vec3 color ;

uniform float near = 1.0; 
uniform float far  = 100.0; 
  
float LinearizeDepth(float depth) {

    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near + z * (near - far));	
}

void main(){         
    
    float depth = LinearizeDepth(gl_FragCoord.z/gl_FragCoord.w); 
    color = vec3(depth);
}
