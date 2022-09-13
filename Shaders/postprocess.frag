#version 330

layout(location = 0) out vec4 f_color;

in vec2 v_texcoords;
float Vignette = length((vec2(-0.5) + v_texcoords) / 2);

uniform sampler2D u_texture;
uniform sampler2D u_outlines;
uniform sampler2D u_enginearrows;
uniform sampler2D u_bloomtexture;
uniform sampler2D u_ssao;
uniform sampler2D u_depth;
uniform float u_gamma;
uniform float u_chrabbsize;
uniform float u_vignette;
uniform bool u_fxaa;
uniform bool u_bloom;

float rand(vec2 co){
	return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

bool IsTexcoordInRange(vec2 Texcoords)
{
	return (Texcoords.x <= 1 && Texcoords.x >= 0 && Texcoords.y <= 1 && Texcoords.y >= 0);
}

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // Back to NDC 
	return (2.0 * 0.1f * 10000.f) / (10000.f + 0.1f - z * (10000.f - 0.1f));
}
float blurssao()
{
	vec2 texelSize = 1.0 / vec2(textureSize(u_ssao, 0));
	float result = 0.0;
	for (int x = -2; x < 2; ++x) 
	{
		for (int y = -2; y < 2; ++y) 
		{
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(u_ssao, v_texcoords + offset).r;
		}
	}


	result = mix(result, 16, min(max(LinearizeDepth(texture(u_depth, v_texcoords).z), 0), 1) / 3);
	result += LinearizeDepth(texture(u_depth, v_texcoords).z) / 5.f;
	return pow(min(result / 16, 1), 2);
}
vec4 blursample(sampler2D tex, vec2 coords)
{
	vec4 color;
	vec2 texelSize = 1.f / textureSize(u_texture, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			color.x += texture2D(u_texture, (vec2(x, y) * texelSize) + v_texcoords + vec2(u_chrabbsize * Vignette)).x;
			color.y += texture2D(u_texture, (vec2(x, y) * texelSize) + v_texcoords + vec2(-u_chrabbsize * Vignette)).y;
			color.z += texture2D(u_texture, (vec2(x, y) * texelSize) + v_texcoords + vec2(u_chrabbsize * Vignette, -u_chrabbsize * Vignette)).z;
		}
	}
	color /= 9;
	return color;
}

vec4 sharpen(in sampler2D tex, in vec2 coords, in vec2 renderSize) {
  float dx = 1.0 / renderSize.x;
  float dy = 1.0 / renderSize.y;
  vec4 sum = vec4(0.0);
  sum += -1. * blursample(tex, coords + vec2( -1.0 * dx , 0.0 * dy));
  sum += -1. * blursample(tex, coords + vec2( 0.0 * dx , -1.0 * dy));
  sum += 5. * blursample(tex, coords + vec2( 0.0 * dx , 0.0 * dy));
  sum += -1. * blursample(tex, coords + vec2( 0.0 * dx , 1.0 * dy));
  sum += -1. * blursample(tex, coords + vec2( 1.0 * dx , 0.0 * dy));
  return sum;
}

void main()
{
	if(IsTexcoordInRange(v_texcoords))
	{
		vec4 color;
		if(u_fxaa)
		color = sharpen(u_texture, v_texcoords, vec2(1));
		else
		{
			color.x += texture2D(u_texture, v_texcoords + vec2(u_chrabbsize * Vignette)).x;
			color.y += texture2D(u_texture, v_texcoords + vec2(-u_chrabbsize * Vignette)).y;
			color.z += texture2D(u_texture, v_texcoords + vec2(u_chrabbsize * Vignette, -u_chrabbsize * Vignette)).z;
		}
		vec2 texelSize = 1.5 / textureSize(u_texture, 0);
		color *= blurssao();
		vec3 outlinecolor = vec3(0.f);
		for(int x = -1; x <= 1; ++x)
		{
			for(int y = -1; y <= 1; ++y)
			{
				if(IsTexcoordInRange((u_outlines, v_texcoords + vec2(x, y) * texelSize)))
				{
					if(abs(LinearizeDepth(texture(u_outlines, v_texcoords).x) -LinearizeDepth(texture(u_outlines, v_texcoords + vec2(x, y) * texelSize).x)) * 100 > 0.1f)
					{
						outlinecolor += vec3(abs(texture(u_outlines, v_texcoords).x - texture(u_outlines, v_texcoords + (vec2(x, y) * texelSize))).x * 50);
					}
				}
			}
		}
		outlinecolor /= vec3(2);
		vec3 bloomcolor;
		if(u_bloom)
		{
		bloomcolor.x = texture2D(u_bloomtexture, v_texcoords + vec2(u_chrabbsize * Vignette)).x;
		bloomcolor.y = texture2D(u_bloomtexture, v_texcoords + vec2(-u_chrabbsize * Vignette)).y;
		bloomcolor.z = texture2D(u_bloomtexture, v_texcoords + vec2(u_chrabbsize * Vignette, -u_chrabbsize * Vignette)).z;
		}
		//color = vec4(vec3(blurssao()), 1);

		float bloomstrength = min(length(bloomcolor), 1);
		vec4 enginearrows = texture(u_enginearrows, v_texcoords);
		f_color = pow(vec4(mix(color.xyz, mix(bloomcolor, vec3(1), 0.5f) * 2.f, bloomstrength) + outlinecolor, color.w), vec4(u_gamma));

		f_color = mix(f_color, enginearrows, length(enginearrows.rgb));
		f_color *= (rand(v_texcoords) / 50) + 0.95; // To combat color banding
		f_color -= Vignette * u_vignette;
	}
	else
	{
		f_color = vec4(0);
	}
}