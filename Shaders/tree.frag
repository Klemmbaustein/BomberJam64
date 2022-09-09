#version 450 core

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
in vec4 v_color;
in vec2 v_texcoord;
in vec3 v_position;
in vec3 v_modelnormal;
in vec3 v_fragposlightspace;
in vec3 v_screenposition;
in vec3 v_normal;
in vec3 v_screennormal;
in float depth;
layout (std140, binding = 0) uniform LightSpaceMatrices
{
	mat4 lightSpaceMatrices[16];
};

uniform sampler2D u_texture;
uniform vec3 u_diffuse;
uniform vec3 u_emissive;
uniform float u_specularsize;
uniform float u_specularstrength;
uniform float u_bias;

uniform int u_shadows;
uniform sampler2DArray shadowMap;

uniform mat4 u_view;
uniform int cascadeCount;
uniform float cascadePlaneDistances[16];
uniform mat4 u_modelviewpro;
uniform mat4 u_modelview;
uniform mat4 u_model;
uniform int u_textureres;
uniform float u_biasmodifier;
uniform int u_usetexture;

uniform DirectionalLight u_directionallight;
uniform float FogFalloff;
uniform float FogDistance;
uniform float FogMaxDensity;
uniform vec3 FogColor;
vec4 ApplyFogColor(vec4 InColor)
{
	return mix(InColor, vec4(FogColor, InColor.w), min(max((depth - FogDistance), 0) / FogFalloff, FogMaxDensity));
}

float SampleFromShadowMap(vec2 Texcoords, float bias, vec2 texelSize, int layer, vec2 distances, float currentDepth)
{
	float pcfDepth;
	pcfDepth = texture(shadowMap, vec3(Texcoords, layer)).r;
	return currentDepth > pcfDepth + bias ? 1.0 : 0.0;
}

float ShadowCalculation(vec3 fragPosWorldSpace)
{
	// select cascade layer
	vec4 fragPosViewSpace = u_view * vec4(fragPosWorldSpace, 1.0);
	float depthValue = abs(fragPosViewSpace.z);

	int layer = -1;
	for (int i = 0; i < cascadeCount; ++i)
	{
		if (depthValue < cascadePlaneDistances[i])
		{
			layer = i;
			break;
		}
	}
	if (layer == -1)
	{
		layer = cascadeCount;
	}
	if(layer == cascadeCount)
	{
		return 0.f;
	}
	vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPosWorldSpace, 1.0);

	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;

	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;

	// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
	if (currentDepth > 1.0)
	{
		return 0.f;
	}
	vec2 texelSize = 1.f / textureSize(shadowMap, 0).xy;

	float bias = 0.0125f;

	bias += (abs(dot(normalize(v_modelnormal), normalize(u_directionallight.Direction)))) / 100;

	bias *= max((abs(u_biasmodifier * 2)), 0.5f) / 15.f;
	if(u_biasmodifier < -0.95)
		bias *= 2;
	bias *= 0.02;
	bias *= max(4096 / textureSize(shadowMap, 0).x*1.5f, 1);

	// PCF
	float shadow = 0.f;
	vec2 distances = vec2(mod(projCoords.x, texelSize.x), mod(projCoords.y, texelSize.y)) * vec2(u_textureres);
	int i = 0;
	for(int x = -1; x <= 1; x += 1)
	{
		for(int y = -1; y <= 1; y += 1)
		{
			{
				shadow += SampleFromShadowMap(projCoords.xy + vec2(x, y) * 1 * texelSize, bias, texelSize, layer, distances, currentDepth);
				i++;
			}
		}
	}
	shadow /= i;
	shadow -= 0.1f;
	shadow = max(shadow, 0);
	shadow *= 1.3f;
	shadow = min(shadow, 1.f);
	return shadow;
}


void main()
{
	vec3 texcolor = texture(u_texture, v_texcoord).xyz * u_diffuse;
	bool transparent = texture(u_texture, v_texcoord).w < 0.33f ? true : false;
	if(transparent)
		discard;
	vec3 ambient = max(vec3(vec4(texcolor, 1.f)) * vec3(u_directionallight.AmbientIntensity),  0.f) * u_directionallight.AmbientColor;
	vec3 diffuse = texcolor  * (1 - u_directionallight.AmbientIntensity) * u_directionallight.SunColor;

	float shadow = 1;
	if(u_shadows == 1)
	{
		shadow = 1;
		shadow = 1 - ShadowCalculation(v_position); //ShadowCalculation is expensive because of PCF
	}
	else
	{
		shadow = 1;
	}

	f_color = ApplyFogColor(vec4((ambient + (diffuse * shadow)), 1));
	brightcolor = vec4(0, 0, 0, 1);
	f_normal = vec4(normalize(v_screennormal), 1);
	f_position = vec4(v_screenposition, 1);
}