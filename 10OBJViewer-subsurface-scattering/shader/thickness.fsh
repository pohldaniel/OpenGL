#version 410 core

void main() {
    float thickness = gl_FragCoord.z/gl_FragCoord.w;
    if (gl_FrontFacing == false) {
        thickness = -thickness;
    }
    
    gl_FragColor = vec4(thickness, 0.0, 0.0, 1.0);
}