#version 430 core

layout(location = 0) out vec4 f_color;
layout(location = 1) out vec4 brightcolor;
layout(location = 2) out vec4 f_normal;
layout(location = 3) out vec4 f_position;
layout(location = 4) out vec4 f_albedo;
struct DirectionalLight
{
	vec3 Direction;
	vec3 Color;
	float Intensity;
};
in vec4 v_color;
in vec2 v_texcoord;
in vec3 v_position;
in vec3 v_modelnormal;
in vec3 v_fragposlightspace;
in vec3 v_screenposition;
in vec3 v_screennormal;

layout (std140, binding = 0) uniform LightSpaceMatrices
{
	mat4 lightSpaceMatrices[16];
};

uniform int u_shadows;
uniform mat4 u_view;
uniform int cascadeCount;
uniform float cascadePlaneDistances[16];
uniform DirectionalLight u_directionallight;



void main()
{
	f_color = vec4(mix(vec3(0.7, 0.8, 1), vec3(0.3f, 0.6f, 1.f), abs(dot(vec3(normalize(v_modelnormal)), vec3(0, 1, 0)))), 1);
	brightcolor = vec4(vec3(f_color.xyz * 0.15f), 1);
	//f_color+= vec4(vec3(250, 250, 250) * max((dot(normalize(-v_modelnormal), normalize(u_directionallight.Direction)) - 0.9985) * 5, 0), 1);
	brightcolor += vec4(vec3(250, 250, 250) * max((dot(normalize(-v_modelnormal), normalize(u_directionallight.Direction)) - 0.9985) * 5, 0), 1);
	f_normal = vec4(vec3(0), 1);
	f_position = vec4(vec3(0), 1);
}