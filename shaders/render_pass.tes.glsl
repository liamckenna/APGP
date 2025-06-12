// Tessellation Evaluation Shader
#version 430
precision highp float;

//layout(triangles, equal_spacing, cw) in;
layout(quads, fractional_even_spacing, cw) in;
//layout(isolines) in;
in vec4 tcPosition[];
in vec4 tcColor[];
in vec2 tcTexCoord[];
in vec4 tcDu[];
in vec4 tcDv[];

out vec4 tePosition;
out vec4 teColor;
out vec2 teUV;
out vec2 tePatchUV;
out vec3 teNormal;
out float teOneOverZ;
out vec4 teDu;
out vec4 teDv;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;

#define DEG 3
#define ORDER (DEG + 1)

void main()
{
	//vec4 teDu, teDv;
	float u = gl_TessCoord.x; 
	float v = gl_TessCoord.y; 
	float w;

	float coeff1d[ORDER];
	float coeff2d[ORDER*ORDER];

	coeff1d[0] = (1-u)*(1-u)*(1-u);
	coeff1d[1] = 3*(u)*(1-u)*(1-u);
	coeff1d[2] = 3*(u)*(u)*(1-u);
	coeff1d[3] = (u)*(u)*(u);

	for(int i = 0; i < ORDER; i++)
	{
		coeff2d[0*ORDER+i] = (1-v)*(1-v)*(1-v)*coeff1d[i];
		coeff2d[1*ORDER+i] = 3*(v)*(1-v)*(1-v)*coeff1d[i];
		coeff2d[2*ORDER+i] = 3*(v)*(v)*(1-v)*coeff1d[i];
		coeff2d[3*ORDER+i] = (v)*(v)*(v)*coeff1d[i];
	}

	w = 0;
	tePosition = vec4(0,0,0,0);
	teColor = vec4(0,0,0,0);
	teUV = vec2(0,0);
	teDu = vec4(0,0,0,0);
	teDv = vec4(0,0,0,0);

	for(int i = 0; i < ORDER*ORDER; i++)
	{
		tePosition += coeff2d[i]*tcPosition[i];
		teColor += coeff2d[i]*tcColor[i];
		teUV += coeff2d[i]*tcTexCoord[i];
		teDu += coeff2d[i]*tcDu[i];
		teDv += coeff2d[i]*tcDv[i];
	}

	vec2 texMin = min(tcTexCoord[0], min(tcTexCoord[3], min(tcTexCoord[12], tcTexCoord[15])));
	vec2 texMax = max(tcTexCoord[0], max(tcTexCoord[3], max(tcTexCoord[12], tcTexCoord[15])));

	// teUV.x = (1-v) * texMin.x + v* texMax.x;
	// teUV.y = (1-u) * texMin.y + u* texMax.y;

	//tePosition = tePosition / tePosition.w;

	//vec3 tmpV[ORDER];

	//for(int i = 0; i < ORDER; i++)
	//{
	//	tmpV[i] = (1-u)*(1-u)*(1-u)*tcPosition[4*i] + 
	//				3*(u)*(1-u)*(1-u)*tcPosition[4*i + 1] +
	//				3*(u)*(u)*(1-u)*tcPosition[4*i + 2] + 
	//				(u)*(u)*(u)*tcPosition[4*i + 3];
	//}

	//tePosition = (1-v)*(1-v)*(1-v)*tmpV[0] + 
	//				3*(v)*(1-v)*(1-v)*tmpV[1] +
	//				3*(v)*(v)*(1-v)*tmpV[2] + 
	//				(v)*(v)*(v)*tmpV[3];

    //teColor = u*tcColor[0] + v*tcColor[1] + w*tcColor[2];
    //tePosition = p0 + p1 + p2;

	//gl_Position = vec4(tePosition, 1.0f);

	vec3 tan_v = (teDv.xyz * tePosition.w - teDv.w * tePosition.xyz) / (tePosition.w * tePosition.w);
	vec3 tan_u = (teDu.xyz * tePosition.w - teDu.w * tePosition.xyz) / (tePosition.w * tePosition.w);

	teDu = vec4(tan_u,1.0);
	teDv = vec4(tan_v,1.0);

	teNormal = NormalMatrix * normalize(cross(tan_v,tan_u));

	tePosition = ModelViewMatrix * vec4(tePosition.xyz / tePosition.w, 1.0f);
	teOneOverZ = abs(1.0f/tePosition.z);
	tePatchUV = vec2(u,v);
	// teColor = vec4(gl_TessCoord.y, 0.0, 0.0,1.0);
	// teNormal = NormalMatrix * normalize(cross(teDv,teDu));
	
	vec4 projectedPosition = ProjectionMatrix * tePosition;
	// teUV = vec2(u,v);
	gl_Position = projectedPosition; 	
}

