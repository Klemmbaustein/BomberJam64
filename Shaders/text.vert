#version 330 core
layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 texcoords;// <vec2 pos, vec2 tex>
out vec2 TexCoords;
out vec2 v_position;
uniform mat4 projection;
uniform vec3 u_offset; //X = Y offset; Y = MaxDistance; Z MinDistance
uniform vec3 transform;
void main()
{
	vec2 pos = vertex; pos *= transform.z;
	pos += transform.xy;
	gl_Position = (projection * vec4(pos, 0.0, 1.0)) + vec4(0, -u_offset.x, 0, 0);
	v_position = gl_Position.xy;
	TexCoords = texcoords;
}