#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoords;

uniform mat4 u_projection;
uniform mat4 u_view;

out vec3 v_color;
out vec2 v_texCoord;

void main(void)
{
   gl_Position =   u_projection * u_view  * vec4(position, 1.0);
   v_texCoord = texCoords;
}