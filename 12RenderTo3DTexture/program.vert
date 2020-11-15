#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 texCoords;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_modelView;

out vec3 v_color;
out vec3 v_texCoord;

void main(void)
{
   gl_Position =   u_projection * u_modelView  * vec4(position, 1.0);
   v_texCoord = texCoords;
}