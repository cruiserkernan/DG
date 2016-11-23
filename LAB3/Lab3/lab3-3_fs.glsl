#version 440

out vec4 frag_colour;
in vec4 normal;
in vec4 position;
in vec2 uv;
in vec4 tangent;
in vec4 bitangent;

uniform sampler2D tex_sampler;
uniform samplerCube cube_sampler;
uniform sampler2D normal_sampler;

uniform int light_count;
uniform vec3 light_position[4]; // can have up to 4 light sources
uniform vec3 light_colour[4];

vec4 camera_position = {0, 0, 1, 1};



float X(float x)
{
	return x > 0 ? 1 : 0;
}

float D(vec3 n, vec3 h, float alpha)
{
	float nDotH = max(dot(n,h),0);
	float alphaSquared = alpha * alpha;
	float nDotHSquared = nDotH * nDotH;
	float denominator = 1 + nDotHSquared * (alphaSquared - 1);
	return alphaSquared/(3.14 * (denominator * denominator));
}

vec3 F0(float IOR)
{
	float oneMinusIOR = 1-IOR;
	float onePlusIOR = 1+IOR;
	return vec3(abs((oneMinusIOR/onePlusIOR)*(oneMinusIOR/onePlusIOR)));
}

vec3 F(vec3 Wo, vec3 n, float IOR)
{
	float cosTheta = max(dot(Wo,n),0);
	float powerToFive = pow((1-cosTheta),5);
	vec3 F0Value = F0(IOR);
	return F0Value + (1-F0Value)*powerToFive;
}


float G(vec3 Wi, vec3 Wo, vec3 h, vec3 n)
{
	float WoDotN = max(dot(Wo, n), 0);
	float hDotN = max(dot(h, n), 0);
	float WoDoth = max(dot(Wo, h), 0);
	float WiDotN = max(dot(Wi, n), 0);
	return min(1, min(2*WoDotN*hDotN/(WoDoth), 2*WiDotN*hDotN/(WoDoth)));
}

vec3 lambertian_brdf( vec3 in_direction, vec3 out_direction, vec3 normal )
{  
  return vec3(1/3.14);
}

vec3 fCookTorrance(vec3 Wi, vec3 Wo, vec3 n)
{
	float IOR = 1.5;
	float alpha = 0.1;


	float WoDotN = max(dot(Wo,n),0);
	float WiDotN = max(dot(Wi,n),0);
	vec3 h = normalize(Wi + Wo);

	return (D(n,h,alpha)*F(Wo,n,IOR)*G(Wi,Wo,h,n))/(4*WoDotN*WiDotN);
}

float oren_nayar(vec3 Wi, vec3 Wo, vec3 n)
{
	float roughness = 0.2;
	float roughnessSquared = roughness*roughness;
	float A = 1.0 - 0.5 * roughnessSquared / (roughnessSquared + 0.57);
	float B = 0.45 * roughnessSquared / (roughnessSquared + 0.09);
	float thetaWi = acos(max(dot(Wi, n), 0));
	float thetaWo = acos(max(dot(Wo, n), 0));
	float alpha = max(thetaWi, thetaWo);
	float beta = min(thetaWi, thetaWo);
	float azimuthDiff = dot(Wo - n * dot(Wo, n), Wi - n * dot(Wi, n)); 

	return 1/3.14 * (A + B * (max(0, azimuthDiff) * sin(alpha) * tan(beta)));
}

vec3 cookTorrance_brdf(vec3 Wi, vec3 Wo, vec3 n, float Kd, float Ks) //Kd + ks <= 1
{
	return Kd * texture(tex_sampler, uv).rgb *  oren_nayar(Wi,Wo,n) + Ks * max(fCookTorrance(Wi,Wo,n), 0); 
	//return fCookTorrance(Wi,Wo,n);
}


float blinn_phong(vec3 Wi, vec3 Wo, vec3 n)
{
	float kL = 0.8;
	float kG = 1-kL;
	float s = 600;
	vec3 h = normalize(Wi + Wo);
	float hDotN = max(dot(h, n), 0);
	return kL / 3.14 + kG * pow(hDotN, s) * (8 + s) / (8 * 3.14); 
}



void main () {

	vec3 sum = vec3(0);
	vec3 normalized_normal = normalize(normal.xyz);
	vec3 Wi;
	vec3 Wr;
	vec3 Wo;
	vec3 bump_normal = normalize((texture(normal_sampler,uv)*2).rgb-1); //Scale
	vec4 normalized_tangent = vec4(normalize(tangent.xyz),0);
	vec4 normalized_bitangent = vec4(normalize(bitangent.xyz),0);
	mat4 TBN = mat4(normalized_tangent,normalized_bitangent,vec4(normalized_normal,0),vec4(0,0,0,1));

	Wo = normalize(vec3(0,0,0) - position.xyz); //0,0,0 since it is in camera space

	bump_normal = (TBN * vec4(bump_normal,0)).xyz;
	// CAREFUL! NORMAL SWITCH!
	normalized_normal = normalize(bump_normal);
	for (int i = 0; i < 1; ++i )
	{
		Wi = normalize(light_position[i] - position.xyz);
		//sum += blinn_phong(Wi, Wo, normalized_normal) * light_colour[i] * max(dot(Wi, normalized_normal), 0);  
		sum += cookTorrance_brdf(Wi, Wo, normalized_normal, 0.3, 0.7) * light_colour[i] * max(dot(Wi, normalized_normal), 0);
	}
	vec3 reflected_view = reflect(-Wo,normalized_normal);
	//frag_colour = vec4(mix(texture(cube_sampler,reflected_view).rgb * F(Wo, normalized_normal, 1.5), sum , 0.5), 1);
	//frag_colour = vec4(texture(normal_sampler,uv).rgb,1);
	frag_colour = vec4(pow(sum.x, 1/2.2), pow(sum.y, 1/2.2), pow(sum.z, 1/2.2),  1);
	//frag_colour = vec4(texture(tex_sampler, vec2(uv.x, uv.y)).rgb, 1);
	//frag_colour = vec4(uv,1,1);
	//frag_colour = texture(normal_sampler,uv);
	
}
