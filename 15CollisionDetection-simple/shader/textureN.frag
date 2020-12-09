#version 410 core

uniform sampler2D u_texture;

in vec2 v_texCoord;
in vec3 v_normal;

out vec4 color;

void main(void)
{
	//color = vec4 (1.0, 0.0, 0.0, 0.0);
	//color = texture2D( u_texture, v_texCoord );
	color = vec4(v_normal, 1.0);
}
