#version 430

struct DirectionalLight
{
	vec3 Direction;
	vec3 SunColor;
	vec3 AmbientColor;
	float Intensity;
	float AmbientIntensity;
};

struct PointLight
{
	vec3 Position;
	vec3 Color;
	float Intensity;
	float Falloff;
	bool Active;
};

uniform sampler2DArray shadowMap;
uniform mat4 u_view;
uniform int cascadeCount = 4;
uniform float cascadePlaneDistances[16];
uniform float u_biasmodifier;
uniform DirectionalLight u_directionallight;
uniform int u_shadowQuality;
uniform int u_textureres;
uniform vec3 u_cameraposition;
uniform float u_specularsize;
uniform float u_specularstrength;
uniform int u_shadows;
uniform PointLight u_lights[16];

layout (std140, binding = 0) uniform LightSpaceMatrices
{
	mat4 lightSpaceMatrices[16];
};

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

float ShadowCalculation(vec3 fragPosWorldSpace, vec3 v_modelnormal)
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

	float bias = 0.0035f;


	bias += (1 - (abs(dot(normalize(v_modelnormal), normalize(u_directionallight.Direction))))) / 75;
	bias *= max((abs(u_biasmodifier * 2)), 0.5f) / 15.f;
	if(u_biasmodifier < -0.95)
		bias *= 2;
	bias *= 0.05;
	bias *= max(4096 / textureSize(shadowMap, 0).x*1.5f, 1);
	// PCF
	float shadow = 0.f;
	vec2 distances = vec2(mod(projCoords.x, texelSize.x), mod(projCoords.y, texelSize.y)) * vec2(u_textureres);
	int i = 0;
	for(int x = 0; x <= 1; x += 1)
	{
		for(int y = 0; y <= 1; y += 1)
		{
			{
				shadow += SampleFromShadowMap(projCoords.xy + vec2(x, y) * (u_shadowQuality - 1) * texelSize, bias, texelSize, layer, distances, currentDepth);
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

vec3 GetLightingAt(vec3 pos, vec3 normal, vec3 color)
{
	vec3 view = normalize(pos.xyz - u_cameraposition.xyz);
	vec3 reflection = reflect(u_directionallight.Direction, normal);
	float shadow = 1;
	if(u_shadows == 1)
	{
		shadow = 1 - ShadowCalculation(pos, normal); //ShadowCalculation is expensive because of PCF
	}
	else
	{
		shadow = 1;
	}
	float specular = 1 * pow(max(dot(reflection, view), 0.000001), u_specularsize * 35) * u_specularstrength;
	vec3 light = (u_directionallight.Direction);
	vec3 DirectionalLightColor = max(vec3(dot(normal, light)), 0.f) * (color * u_directionallight.Intensity) * (1 - u_directionallight.AmbientIntensity) * u_directionallight.SunColor;
	vec3 ambient = max(vec3(vec4(color, 1.f)) * vec3(u_directionallight.AmbientIntensity),  0.f) * u_directionallight.AmbientColor;

	vec3 lightingColor;
	for(int i = 0; i < 16; i++)
	{
		if(u_lights[i].Active)
		{
			vec3 pointLightDir = (u_lights[i].Position - pos) / u_lights[i].Falloff;
			lightingColor += min(max(dot(normal, normalize(pointLightDir)), 0)
				* u_lights[i].Color / pow(length(pointLightDir), 6) * u_lights[i].Intensity / u_lights[i].Falloff, 1);
		}
		else break;
	}

	return ambient + (DirectionalLightColor + specular) * shadow + lightingColor;
}
