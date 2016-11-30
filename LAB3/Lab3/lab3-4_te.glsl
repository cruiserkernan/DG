//Tesselation Evaluation Shader
#version 440

// We work with triangle patches
layout(triangles) in;

// Inputs from TCS
in vec3 tcPosition[];
in vec2 tcTexCoord[];

// Outputs, to Fragment Shader
out vec4 tePosition;
out vec3 teNormal;
out vec2 teTexCoord;

// This is our displacement map
uniform sampler2D heightmap;

// We'll finally apply these uniforms now 
uniform mat4 normalMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;


vec4 gaussian_sample(const sampler2D tex, const vec2 uv, const float radius, const vec2 a, const vec2 b)
{
  //sampling is done along the uv directions, with attention to scale,
  //for non-square textures.
  vec2 ts = vec2(length(a), length(b))*radius;
  
  return	(1.0/4.0)  * texture(tex,uv) +
    (1.0/8.0)  * texture(tex,uv + a*ts.x) +
    (1.0/8.0)  * texture(tex,uv - a*ts.x) +
    (1.0/8.0)  * texture(tex,uv + b*ts.y) +
    (1.0/8.0)  * texture(tex,uv - b*ts.y) +
    (1.0/16.0) * texture(tex,uv + a*ts.x + b*ts.y) +
    (1.0/16.0) * texture(tex,uv + a*ts.x - b*ts.y) +
    (1.0/16.0) * texture(tex,uv - a*ts.x + b*ts.y) +
    (1.0/16.0) * texture(tex,uv - a*ts.x - b*ts.y);
}

const float sample_offset = 0.01;
const float displacement_coef = -0.1;

void main()
{

  // 1. Compute interpolated Position and TexCoord using gl_TessCoord
  // with tcPosition and tcTexCoord, respectively.
  vec3 p1 = tcPosition[0];
  vec3 p2 = tcPosition[1];
  vec3 p3 = tcPosition[2];

  vec2 u1 = tcTexCoord[0];
  vec2 u2 = tcTexCoord[1];
  vec2 u3 = tcTexCoord[2];
  
  vec3 tmpPosition = gl_TessCoord.x * tcPosition[0] +
                    gl_TessCoord.y * tcPosition[1] +
                    gl_TessCoord.z * tcPosition[2];

  vec3 v = tmpPosition;
  vec2 teTexCoordTmp = gl_TessCoord.x * tcTexCoord[0] +
                      gl_TessCoord.y * tcTexCoord[1] +
                      gl_TessCoord.z * tcTexCoord[2];



  // vec4 tmpPosition = ...;
  // teTexCoord = ...;

  // 2. Compute the normal for the triangle that connects tcPosition[]
   vec3 U = tcPosition[1] - tcPosition[0];
   vec3 V = tcPosition[2] - tcPosition[0];
	 vec3 normal = normalize(cross(U, V).xyz);

  // 3. Compute the amount of displacement for the vertex by using the
  // sampler2D heightmap
  float d = texture(heightmap,teTexCoordTmp).y * displacement_coef;
  tmpPosition += d*normal; 

  // 4a. Select 2 other texture coordinates and compute the
  // displacement at those points. >Okay.

  vec3 e1 = p2 - p1;
  vec3 e2 = p3 - p1;

  vec2 t1 = u2 - u1;
  vec2 t2 = u3 - u1;

  vec3 v1 = v + sample_offset * length(e1) * e1;
  vec3 v2 = v + sample_offset * length(e2) * e2;

  vec2 v1Tex = teTexCoordTmp + sample_offset * length(t1) * t1;
  vec2 v2Tex = teTexCoordTmp + sample_offset * length(t2) * t2;

  float d2 = texture(heightmap,v1Tex).y * displacement_coef;
  float d3 = texture(heightmap,v2Tex).y * displacement_coef;

  vec3 displacedCoordinate2 = v1 + d2*normal;
  vec3 displacedCoordinate3 = v2 + d3*normal;

  // 4b. Use the three displaced points to compute a new normal, and
  // put it in teNormal;
   vec3 U2 = displacedCoordinate2 - tmpPosition;
   vec3 V2 = displacedCoordinate3 - tmpPosition;
	 normal = normalize(cross(U2, V2));

  //teNormal = (normalMatrix * vec4(normal, 0)).xyz;
  // 5. Now apply transformations  
  teTexCoord = teTexCoordTmp;
  teNormal = (normalMatrix * vec4(normal, 0)).xyz;
  gl_Position = modelViewProjectionMatrix * vec4(tmpPosition, 1);
  tePosition = modelViewMatrix*vec4(tmpPosition, 1);
}
