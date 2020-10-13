#version 330

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;

out vec2 fTexCoord;

void main(void) {
    gl_Position =  vec4(i_position, 1.0);

    fTexCoord  = i_texCoord;
}
