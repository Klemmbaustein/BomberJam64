#version 330 core
out float FragColor;
  
in vec2 v_texcoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;
uniform int kernelSize;
uniform float radius;
uniform vec3 samples[64];
uniform mat4 projection;
uniform int ResDiv;
uniform vec2 screenRes;
float bias = 0.1;

// tile noise texture over screen, based on screen dimensions divided by noise size
vec2 noiseScale = screenRes / 4 / ResDiv; 


void main()
{
	vec2 texcoords = v_texcoords * ResDiv;
	// get input for SSAO algorithm
	vec3 fragPos = texture(gPosition, texcoords).xyz;
	vec3 normal = normalize(texture(gNormal, texcoords).rgb);
	if(normal == vec3(0))
	discard;
	vec3 randomVec = normalize(texture(texNoise, texcoords * noiseScale).xyz);
	// create TBN change-of-basis matrix: from tangent-space to view-space
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);
	// iterate over the sample kernel and calculate occlusion factor
	float occlusion = 0.0;
	for(int i = 0; i < kernelSize; ++i)
	{
		// get sample position
		vec3 samplePos = TBN * samples[i]; // from tangent to view-space
		samplePos = fragPos + samplePos * radius; 
		
		// project sample position (to sample texture) (to get position on screen/texture)
		vec4 offset = vec4(samplePos, 1.0);
		offset = projection * offset; // from view to clip-space
		offset.xyz /= offset.w; // perspective divide
		offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
		
		if(offset.x > 0 && offset.x < 1 && offset.y > 0 && offset.y < 1)
		{
			// get sample depth
			float sampleDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample
		
			// range check & accumulate
			float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
			occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
		}
	}
	occlusion = 1.0 - (occlusion / kernelSize);
	
	FragColor = occlusion;
}