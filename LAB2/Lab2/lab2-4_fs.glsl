#version 440

out vec4 frag_colour;
in vec4 normal;
in vec4 position;

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
	float alpha = 0.05;


	float WoDotN = max(dot(Wo,n),0);
	float WiDotN = max(dot(Wi,n),0);
	vec3 h = normalize(Wi + Wo);

	return (D(n,h,alpha)*F(Wo,n,IOR)*G(Wi,Wo,h,n))/(4*WoDotN*WiDotN);
}

vec3 cookTorrance_brdf(vec3 Wi, vec3 Wo, vec3 n, float Kd, float Ks) //Kd + ks <= 1
{
	return Kd * lambertian_brdf(Wi,Wo,n) + Ks * max(fCookTorrance(Wi,Wo,n), 0); 
	//return fCookTorrance(Wi,Wo,n);
}


void main () {

	vec3 sum = vec3(0);
	vec3 normalized_normal = normalize(normal.xyz);
	vec3 Wi;
	vec3 Wr;
	vec3 Wo;

	Wo = normalize(vec3(0,0,0) - position.xyz); //0,0,0 since it is in camera space

	for (int i = 0; i < light_count; ++i )
	{
		Wi = normalize(light_position[i] - position.xyz);
		sum += cookTorrance_brdf(Wi, Wo, normalized_normal, 0.7, 0.3) * light_colour[i] * max(dot(Wi, normalized_normal), 0);    
	}
	frag_colour = vec4(pow(sum.x, 0.455), pow(sum.y, 0.455), pow(sum.z, 0.455), 1);
}
