#version 330

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

out vec4 fPosScreen;
out vec3 fPosWorld;
out vec3 fNormal;
out vec2 fTexCoord;

uniform mat4 uMVPMat;

void main(void) {
    gl_Position = uMVPMat * vec4(i_position, 1.0);
    fPosScreen  = gl_Position;
    fPosWorld   = i_position;
    fNormal     = i_normal;
    fTexCoord   = i_texCoord;
}
