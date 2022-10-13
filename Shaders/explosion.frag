//! #include "shared.frag"

layout(location = 0) out vec4 f_color;
layout(location = 1) out vec4 brightcolor;
layout(location = 2) out vec4 f_normal;
layout(location = 3) out vec4 f_position;
layout(location = 4) out vec4 f_albedo;


in vec2 v_texcoord;
in vec3 v_position;
in vec3 v_modelnormal;
in vec3 v_fragposlightspace;
in vec3 v_screenposition;
in vec3 v_normal;
in vec3 v_screennormal;
in float depth;

uniform sampler2D u_texture;
uniform vec3 u_diffuse;
uniform vec3 u_emissive;
uniform mat4 u_modelviewpro;
uniform mat4 u_modelview;
uniform mat4 u_model;
uniform int u_usetexture;
uniform int u_ssao;
uniform float u_opacity;

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
	vec3 texcolor;
	vec3 normal = normalize(v_modelnormal);
	bool transparent = false;
	if(u_usetexture == 1)
	{
		texcolor = u_diffuse * texture(u_texture, v_texcoord).rgb + vec3(0.2, 0.2, 0.2);
		transparent = texture(u_texture, v_texcoord).w < 0.33f ? true : false;
	}
	else
		texcolor = vec3(1.5, 1.2, 0.5);
	if(transparent)
		discard;
	f_color = vec4(texcolor, u_opacity);
	brightcolor = vec4(texcolor * 3, u_opacity);
	if(u_ssao == 1)
	{
		f_normal = vec4(normalize(v_screennormal), 1);
		f_position = vec4(v_screenposition, 1);
	}
	else
	{
		f_normal = vec4(0);
		f_position = vec4(0);
	}
}