#version 410 core

const int MAX_JOINTS = 50;
const int MAX_WEIGHTS = 3;

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in ivec4 i_jointIds;
layout(location = 4) in vec4 i_jointWeights;

out vec2 v_texCoord;
out vec3 v_normal;

uniform mat4 jointTransforms[MAX_JOINTS];

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    vec4 totalLocalPos = vec4(0.0);
    vec4 totalNormal = vec4(0.0);

    for (int i = 0; i< MAX_WEIGHTS; i++) 
    {		
        mat4 jointTransform = jointTransforms[i_jointIds[i]];

        vec4 posePosition = jointTransform * vec4(i_position, 1.0);
        totalLocalPos += posePosition * i_jointWeights[i];

        vec4 worldNormal = jointTransform * vec4(i_normal, 0.0);
        totalNormal += worldNormal * i_jointWeights[i];
    }

	gl_Position = u_projection * u_view * u_model * totalLocalPos;
	v_texCoord = i_texCoord;
    v_normal = totalNormal.xyz;
	//v_normal = (totalLocalPos * vec4(i_normal, 0.0)).xyz;
}