#version 410 core

in vec4 vPosLS;



void main(void){

    gl_FragColor =  vec4(vec3(vPosLS.z* 0.5 + 0.5), -sign(abs(vPosLS.x)) );
}