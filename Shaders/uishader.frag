#version 330

in vec2 v_texcoords;
in vec2 v_position;
layout(location = 0) out vec4 f_color;

uniform vec4 u_color;
uniform int u_usetexture;
uniform sampler2D u_texture;
uniform vec3 u_offset; //X = Y offset; Y = MaxDistance; Z MinDistance
uniform int u_onlyalphafortexture;
void main()
{
	if(u_offset.y > v_position.y)
	{
		discard;
	}
	if(u_offset.z < v_position.y)
	{
		discard;
	}
	if(u_usetexture == 1)
	{
		if(u_onlyalphafortexture == 0)
		{
			f_color = vec4(u_color) * texture(u_texture, v_texcoords);
		}
		else
		{
			f_color = vec4(u_color) * texture(u_texture, v_texcoords).w;
		}
	}
	else
	{
		f_color = vec4(u_color.xyz, 1.f);
	}
}