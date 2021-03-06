#version 330 core 

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 normalData;
layout (location = 2) out vec4 positionData;
layout (location = 3) out vec4 specularData;
layout (location = 4) out vec4 eyeNormalData;
layout (location = 5) out vec4 eyePositionData;
layout (location = 6) out vec4 albedoTextureOut;
layout (location = 7) out vec4 metallicRoughnessAoOut;

in vec2 TexCoords;
in vec2 vs_textureCoordinates;
in vec3 vs_normalData;
in vec3 vs_pos;
in vec3 eyePos;
in vec3 eyeNormal;
in mat3 TBN;
in vec3 tangentViewPos;
in vec3 tangentFragPos;
in vec3 aTan;
in vec3 aBitan;
in vec3 aNorm;
in vec3 Normal;
in vec3 WorldPos;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_diffuse4;

uniform sampler2D texture_specular0;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform sampler2D texture_specular3;
uniform sampler2D texture_specular4;

uniform sampler2D texture_normal0;
uniform sampler2D texture_normal1;
uniform sampler2D texture_normal2;
uniform sampler2D texture_normal3;
uniform sampler2D texture_normal4;

uniform sampler2D texture_displacement0;

uniform sampler2D texture_displacement; 
uniform sampler2D texture_ao;
uniform sampler2D texture_normal;
uniform sampler2D texture_roughness;
uniform sampler2D texture_metallic;
uniform sampler2D texture_diffuse;

uniform sampler2D albedoTex;
uniform sampler2D metallicTex;
uniform sampler2D normalTex; 
uniform sampler2D roughnessTex;

uniform float m_roughness;
uniform float m_metallic;

uniform int aoPresent;
uniform int roughnessPresent;

uniform sampler2D atexture;


uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;  

uniform vec3 lightPositions[4];

uniform vec3 camPos;


const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness){
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
void main()
{	
	vec3 albedo = pow(texture(texture_diffuse, TexCoords).rgb, vec3(2.2));
	float metallic = texture(texture_metallic, TexCoords).r;
	
	float roughness = 0.5;
	if(roughnessPresent){
		roughness = texture(texture_roughness, TexCoords).r;
	}

	float ao = 1.0;
	if(aoPresent){
		ao = texture(texture_ao, TexCoords).r;
	}
	//float ao = texture(texture_ao, TexCoords).r;
	//float ao = 1.0;
	//vec3 albedo = vec3(0.5,0.0,0.0);
	//float metallic = 0.8;
	//float roughness = 0.2; 
	//float ao = 1.0;

	vec3 tangentNormal = texture(texture_normal, TexCoords).xyz * 2.0 - 1.0; 

	vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    //vec3 N   = normalize(Normal);
    //vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    //vec3 B  = -normalize(cross(N, T));
    //mat3 TBN = mat3(T, B, N);

	//N = normalize(TBN * tangentNormal);


	vec3 N = texture(texture_normal, TexCoords).rgb; 
	N = N * 2.0 - 1.0; 
	N = normalize(N); 

	//N = vec3(Normal);

	vec3 V = normalize(camPos - WorldPos);
	vec3 R = reflect(V, -N);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0);

	vec3 F = fresnelSchlickRoughness(max(dot(N,V), 0.0), F0, roughness);
	vec3 kS = F; 
	vec3 kD = 1.0 - kS; 
	kD *= 1.0 - metallic;

	vec3 irradiance = texture(irradianceMap, N).rgb;
	vec3 diffuse = irradiance * albedo; 

	//specular IBL 
	const float MAX_REFLECTION_LOD = 4.0; 
	vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb; 
	vec2 brdf = texture(brdfLUT, vec2(max(dot(N,V), 0.0), roughness)).rg; 
	vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

	vec3 ambient = (kD * diffuse + specular) * ao;

	vec3 color = ambient + Lo; 

	FragColor = vec4(color, 1.0);

	specularData = vec4(specular, 1.0);

	//Output params 
	positionData = vec4(WorldPos, 1.0);

	eyePositionData = vec4(eyePos,1.0);

	//vec3 ey = 0.5f * normalize(eyeNormal) + 0.5f;
	//eyeNormalData = vec4(ey,1.0);
	eyeNormalData = vec4(eyeNormal, 1.0);
	normalData = normalize(vec4(N,1.0));


	albedoTextureOut = vec4(albedo,1.0); 
	metallicRoughnessAoOut = vec4(metallic, roughness, ao, 1);
}

