#version 430 core

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
in vec3 v_modelnormal;
in vec3 v_fragposlightspace;
in vec3 v_screenposition;
in vec3 v_normal;
in vec3 v_screennormal;
in vec3 v_normmodelnormal;

layout (std140, binding = 0) uniform LightSpaceMatrices
{
	mat4 lightSpaceMatrices[16];
};

uniform float u_specularsize;
uniform float u_specularstrength;
in float depth;

uniform sampler2D u_texture;
uniform vec3 u_diffuse;
uniform vec3 u_emissive;
uniform int u_textureres;
uniform int u_shadows;
uniform sampler2DArray shadowMap;
uniform int u_shadowQuality;
uniform mat4 u_view;
uniform int cascadeCount;
uniform float cascadePlaneDistances[16];
uniform mat4 u_modelviewpro;
uniform mat4 u_modelview;
uniform mat4 u_model;
uniform int u_usetexture;
uniform float u_biasmodifier;
uniform DirectionalLight u_directionallight;
uniform vec3 u_cameraposition;
float SampleFromShadowMap(vec2 Texcoords, float bias, vec2 texelSize, int layer, vec2 distances, float currentDepth)
{
	int i = 0;
	float points[4];

	for(int x = 0; x <= 1; ++x)
	{
		for(int y = 0; y <= 1; ++y)
		{
			float pcfDepth;
			pcfDepth = texture(shadowMap, vec3(Texcoords + vec2(x, y) * texelSize.rg, layer)).r;
			points[i] = currentDepth > pcfDepth + bias ? 1.0 : 0.0;
			i++;
		}
	}
	return mix(mix(points[0], points[2], distances.x), mix(points[1], points[3], distances.x), distances.y);
}
float SampleFromShadowMapCheap(vec2 Texcoords, float bias, int layer, float currentDepth)
{
	float pcfDepth;
	pcfDepth = texture(shadowMap, vec3(Texcoords, layer)).r;
	return currentDepth > pcfDepth + bias ? 1.0 : 0.0;
}

uniform float FogFalloff;
uniform float FogDistance;
uniform float FogMaxDensity;
uniform vec3 FogColor;
vec4 ApplyFogColor(vec4 InColor)
{
	return mix(InColor, vec4(FogColor, InColor.w), min(max((depth - FogDistance), 0) / FogFalloff, FogMaxDensity));
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

	bias += (1 - (abs(dot(normalize(v_modelnormal), normalize(u_directionallight.Direction))))) / 50;

	bias *= max((abs(u_biasmodifier * 2)), 0.5f) / 15.f;
	if(u_biasmodifier < -0.95)
		bias *= 2;
	bias *= 0.04;
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
				shadow += SampleFromShadowMap(projCoords.xy + vec2(x, y) * (u_shadowQuality - 1) * texelSize, bias, texelSize, layer, distances, currentDepth);
				i++;
			}
		}
	}
	if(shadow != 8 && shadow != 0)
	{
		for(int x = -u_shadowQuality; x <= u_shadowQuality; ++x)
		{
			for(int y = -u_shadowQuality; y <= u_shadowQuality; ++y)
			{
				if(abs(x) > 1 && abs(x) > 1);
				{
					shadow += SampleFromShadowMap(projCoords.xy + vec2(x, y) * (u_shadowQuality - 1) * texelSize, bias, texelSize, layer, distances, currentDepth);
					//shadow += SampleFromShadowMapCheap(projCoords.xy + vec2(x, y) * texelSize, bias, layer, currentDepth);
					i++;
				}
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
	vec3 texcolor;
	vec3 normal = normalize(v_modelnormal);
	bool transparent = false;
	if(u_usetexture == 1)
	{
		texcolor = u_diffuse * texture(u_texture, v_position.rb / 25).rgb * 2;
		transparent = texture(u_texture, v_texcoord).w < 0.33f ? true : false;
	}
	else
		texcolor = u_diffuse;
	if(transparent)
		discard;
	vec3 view = normalize(v_position.xyz - u_cameraposition.xyz);
	vec3 reflection = reflect(u_directionallight.Direction, normal);
	float specular = 1 * pow(max(dot(reflection, view), 0.000001), u_specularsize * 35) * u_specularstrength;
	vec3 light = (u_directionallight.Direction);
	vec3 ambient = max(vec3(vec4(texcolor, 1.f)) * vec3(u_directionallight.AmbientIntensity),  0.f) * u_directionallight.AmbientColor;
	vec3 diffuse = max(vec3(dot(normal, light)), 0.f) * (texcolor * u_directionallight.Intensity) * (1 - u_directionallight.AmbientIntensity) * u_directionallight.SunColor;

	float shadow = 1;
	if(u_shadows == 1)
	{
		shadow = 1 - ShadowCalculation(v_position); //ShadowCalculation is expensive because of PCF
	}
	else
	{
		shadow = 1;
	}

	f_color = ApplyFogColor(vec4((ambient + (diffuse + specular) * shadow + u_emissive), 1));
	brightcolor = min(vec4(min(pow(diffuse * shadow + max(u_emissive - vec3(1), 0), vec3(25)), 5), 1.0), vec4(2, 2, 2, 1.f));
	brightcolor += specular * shadow;
	f_normal = vec4(normalize(v_screennormal), 1);
	f_position = vec4(v_screenposition, 1);
}