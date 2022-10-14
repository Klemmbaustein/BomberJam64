//! #include "shared.frag"

layout(location = 0) out vec4 f_color;
layout(location = 1) out vec4 brightcolor;
layout(location = 2) out vec4 f_normal;
layout(location = 3) out vec4 f_position;
layout(location = 4) out vec4 f_albedo;
struct DirectionalLight
{
	vec3 Direction;
	vec3 SunColor;
	vec3 AmbientColor;
	float Intensity;
	float AmbientIntensity;
};
in vec2 v_texcoord;
in vec3 v_position;
in vec3 v_fragposlightspace;
in vec3 v_screenposition;
in vec3 v_normal;
in vec4 u_modelviewposition;
in vec3 v_modelnormal;
in vec3 v_screennormal;
in float depth;


uniform vec3 u_diffuse;
uniform vec3 u_emissive;
uniform float u_bias;

uniform mat4 u_modelviewpro;
uniform mat4 u_modelview;
uniform mat4 u_model;
uniform vec3 u_cameraforward;
uniform sampler2D u_normal;
uniform vec3 u_color;
uniform float u_time;
uniform float FogFalloff;
uniform float FogDistance;
uniform float FogMaxDensity;
uniform vec3 FogColor;
vec4 ApplyFogColor(vec4 InColor)
{
	return mix(InColor, vec4(FogColor, InColor.w), min(max((depth - FogDistance), 0) / FogFalloff, FogMaxDensity));
}

void main()
{
	vec3 view = normalize(v_position.xyz - u_cameraposition.xyz);
	vec3 normal = texture(u_normal, v_texcoord * 5.f).rgb;
	normal = normalize(normal * 2.0 - 1.0f);
	normal = vec3(normal.x, normal.y, 1 -normal.z);
	normal = normalize(v_normal + normal);

	vec3 color = GetLightingAt(v_position, normal, vec3(0.7, 1, 1.5));

	f_color = ApplyFogColor(vec4(color, 1));
	brightcolor = vec4(vec3(0), 1);
	f_normal = vec4(normalize(v_screennormal), 1);
	f_position = vec4(v_screenposition, 1);
}