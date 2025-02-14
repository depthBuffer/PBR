#version 330 core
out vec4 FragColor;

in VS_OUT
{
	vec3 worldPos;
    vec2 texCoords;
	vec3 normal;
}fs_in;

uniform samplerCube IrradianceMap;
uniform samplerCube PrefilterMap;
uniform sampler2D brdfLUT;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

uniform vec3 lightPos[4];
uniform vec3 lightCol[4];
uniform vec3 camPos;

const float PI = 3.14159265359;
const float MAX_REFLECTION_LOD = 4.0;

vec3 GetNormalFromMap()
{
	vec3 tangentNormal = texture(normalMap, fs_in.texCoords).rgb * 2.0 - 1.0;

	vec3 Q1 = dFdx(fs_in.worldPos);
	vec3 Q2 = dFdy(fs_in.worldPos);
	vec2 st1 = dFdx(fs_in.texCoords);
	vec2 st2 = dFdy(fs_in.texCoords);

	vec3 N = normalize(fs_in.normal);
	vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / max(denom, 0.001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}

vec3 FresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (1.0 - F0) * pow(1 - cosTheta, 5.0);
}

vec3 FresnelSchlickRoughness(vec3 F0, float cosTheta, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
	vec3 albedo = pow(texture(albedoMap, fs_in.texCoords).rgb, vec3(2.2));
	float metallic = texture(metallicMap, fs_in.texCoords).r;
	float roughness = texture(roughnessMap, fs_in.texCoords).r;
	float ao = texture(aoMap, fs_in.texCoords).r;
	
	vec3 N = GetNormalFromMap();
	vec3 V = normalize(camPos - fs_in.worldPos);
	vec3 R = reflect(-V, N);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0);
	for (int i = 0; i < 4; i++)
	{
		vec3 L = normalize(lightPos[i] - fs_in.worldPos);
		vec3 H = normalize(V + L);
		float distance = length(lightPos[i] - fs_in.worldPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightCol[i] * attenuation;

		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = FresnelSchlick(F0, max(dot(H, V), 0.0));

		vec3 nominator = NDF * G * F;
		float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
		vec3 specular = nominator / denominator;

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		float NdotL = max(dot(N, L), 0.0);

		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}
    
	vec3 F = FresnelSchlickRoughness(F0, max(dot(N, V), 0.0), roughness);
	
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;
	
	vec3 irradiance = texture(IrradianceMap, N).rgb;
	vec3 diffuse = irradiance * albedo;

	vec3 prefilteredColor = textureLod(PrefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

	vec3 ambient = (kD * diffuse + specular) * ao;
    
	vec3 color = ambient + Lo;

	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0 / 2.2));

	FragColor = vec4(color, 1.0);
}