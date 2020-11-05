#version 410 core

in vec2 v_texCoord;
in vec3 v_normal;

uniform sampler2D diffuse2;

void main()
{
    gl_FragColor = texture2D(diffuse2, v_texCoord);
		//*clamp(dot(-vec3(0, 0, 1), v_normal), 0.0, 1.0);
}