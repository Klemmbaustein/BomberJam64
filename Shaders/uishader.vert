#version 330

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoords;
uniform vec3 u_offset; //X = Y offset; Y = MaxDistance
out vec2 v_position;
out vec2 v_texcoords;

void main()
{
	gl_Position = vec4(a_position + vec3(0, -u_offset.x, 0), 1.f);
	v_texcoords = a_texcoords;
	v_position = gl_Position.xy;
}