#version 410 core


layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 3) in vec3 i_normal;

out vec4 fPosScreen;
out vec3 fPosCamera;
out vec3 fNrmCamera;
out vec2 fTexCoord;
out vec3 fLightPos;

uniform mat4 u_projection;
uniform mat4 u_view;

uniform mat4 uMVPMat;
uniform mat4 uMVMat;
uniform vec3 uLightPos;

void main(void) {
    gl_Position = uMVPMat * vec4(i_position, 1.0);
    fPosScreen = gl_Position;

    fPosCamera = (uMVMat * vec4(i_position, 1.0)).xyz;
    fNrmCamera = (transpose(inverse(uMVMat)) * vec4(i_normal, 0.0)).xyz;
    fTexCoord  = i_texCoord;
    fLightPos  = (uMVMat * vec4(uLightPos, 1.0)).xyz;
}