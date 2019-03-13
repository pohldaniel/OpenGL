#version 410 core

uniform mat4 modelview;
uniform mat4 projection;

uniform mat4 u_modelView;
uniform mat4 u_projection;

attribute vec4 position;
attribute vec3 normal;

layout(location = 0) in vec3 i_position;
layout(location = 3) in vec3 i_normal;


void main() {

    gl_Position = u_projection * u_modelView  * vec4(i_position, 1.0);
}