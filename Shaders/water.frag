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
in vec2 v_texcoord;
in vec3 v_position;
in vec3 v_fragposlightspace;
in vec3 v_screenposition;
in vec3 v_normal;
in vec4 u_modelviewposition;
in vec3 v_modelnormal;
in vec3 v_screennormal;
in float depth;

layout (std140, binding = 0) uniform LightSpaceMatrices
{
	mat4 lightSpaceMatrices[16];
};

uniform vec3 u_diffuse;
uniform vec3 u_emissive;
uniform float u_specularsize;
uniform float u_specularstrength;
uniform float u_bias;

uniform int u_shadows;
uniform sampler2DArray shadowMap;
uniform int u_shadowQuality;
uniform mat4 u_view;
uniform int cascadeCount;
uniform float cascadePlaneDistances[16];
uniform mat4 u_modelviewpro;
uniform mat4 u_modelview;
uniform mat4 u_model;
uniform int u_textureres;
uniform float u_biasmodifier;
uniform DirectionalLight u_directionallight;
uniform vec3 u_cameraforward;
uniform vec3 u_cameraposition;
uniform sampler2D u_normal;
uniform sampler2D u_texture;
uniform float u_time;
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

	float bias = 0.015f;

	bias += (abs(dot(normalize(v_modelnormal), normalize(u_directionallight.Direction)))) / 100;
//	if(layer < 3)
//		bias -= min(layer, 26) / 350.f;
//	else
//		bias -= min(layer, 10) / 225.f;
	bias *= max((abs(u_biasmodifier * 2)), 0.5f) / 15.f;
	if(u_biasmodifier < -0.95)
		bias *= 2;
	bias *= 0.05;
	bias *= (4096 / textureSize(shadowMap, 0).x);

	// PCF
	float shadow = 0.f;
	vec2 distances = vec2(mod(projCoords.x, texelSize.x), mod(projCoords.y, texelSize.y)) * vec2(u_textureres);
	int i = 0;
	shadow += SampleFromShadowMap(projCoords.xy, bias, texelSize, layer, distances, currentDepth);
	shadow -= 0.1f;
	shadow = max(shadow, 0);
	shadow *= 1.3f;
	shadow = min(shadow, 1.f);
	return shadow;
}

void main()
{
	vec3 view = normalize(v_position.xyz - u_cameraposition.xyz);
	vec3 normal = texture(u_normal, v_texcoord * 1000.f + (u_time / 3 + sin(u_time) * 0.25) / 10).rgb;
	normal = normalize(normal * 2.0 - 1.0f);
	normal = vec3(normal.x, normal.y, 1 -normal.z);
	normal = normalize(v_normal + normal);
	float shadow = 1 - ShadowCalculation(v_position);
	vec3 reflection = reflect(u_directionallight.Direction, normal);
	float specular = 1 * pow(max(dot(reflection, view), 0.0000001), 50) * 1;

	vec3 color = texture(u_texture, v_texcoord * 2000.f + (u_time / 3 + sin(u_time) * 0.25) / 10).xyz;
	color = mix(color, vec3(0.5, 0.75, 1), min(v_screenposition.z / -500, 1));

	f_color = ApplyFogColor(vec4((color * u_directionallight.SunColor + specular) * shadow + color * u_directionallight.AmbientColor / 5 + max(specular - 0.5, 0) / 5, 1));
	brightcolor = vec4(vec3(specular * shadow), 1);
	f_normal = vec4(normalize(v_screennormal), 1);
	f_position = vec4(v_screenposition, 1);
}