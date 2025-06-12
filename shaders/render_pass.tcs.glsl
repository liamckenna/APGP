// TessControl Shader
#version 430

layout(vertices = 16) out;
in vec4 vPosition[];
in vec4 vColor[];
in vec2 vTexCoord[];

out vec4 tcPosition[];
out vec4 tcDu[];
out vec4 tcDv[];
out vec4 tcColor[];
out vec2 tcTexCoord[];

uniform float TessLevelInner;
uniform float TessLevelOuter;
uniform int patch_offset;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

// layout (binding=4) uniform sampler1D tess_levels;
layout (binding=5) uniform isampler1D patch_connectivity;

layout(std430, binding = 2) readonly buffer PatchBuffer { 
	float patch_tess_levels[ ]; // unsized array allowed at end of buffer 
};

#define ID gl_InvocationID

void main()
{

	// calculate the control point for derivitive calculation
	// FIXME: is there a better approach?
	
	int row, column;
	vec4 Du[3],Dv[3];

	row = ID/4;
	column = ID%4;

	for(int i = 0; i < 3; i++)
	{
		Du[i] = vPosition[row*4 + i + 1] - vPosition[row*4 +i];
		Dv[i] = vPosition[(i+1)*4 + column] - vPosition[i*4 + column];
	}

	switch(column)
	{
	case 0:
		tcDu[ID] = Du[0];
		break;
	case 1:
		tcDu[ID] = (Du[0] + 2 * Du[1])/3.0f;
		break;
	case 2:
		tcDu[ID] = (2* Du[1] + Du[2])/3.0f;
		break;
	case 3:
		tcDu[ID] = Du[2];
		break;
	}

	switch(row)
	{
	case 0:
		tcDv[ID] = Dv[0];
		break;
	case 1:
		tcDv[ID] = (Dv[0] + 2 * Dv[1])/3.0f;
		break;
	case 2:
		tcDv[ID] = (2* Dv[1] + Dv[2])/3.0f;
		break;
	case 3:
		tcDv[ID] = Dv[2];
		break;
	}
	
	

	// pass down other properties
    tcPosition[ID] = vPosition[ID];
    tcColor[ID] = vColor[ID];
	tcTexCoord[ID] = vTexCoord[ID];

	if(gl_PrimitiveID == 0)
		tcColor[ID] = vec4(1,0,0,1);
	else
		tcColor[ID] = vec4(0,1,0,1);

    if (ID == 0) {

		float local_tess_level = patch_tess_levels[gl_PrimitiveID + patch_offset];

		
		gl_TessLevelInner[0] = local_tess_level;
		gl_TessLevelInner[1] = local_tess_level;

		ivec4 neighbour = ivec4(-1,-1,-1,-1);

		neighbour = texelFetch(patch_connectivity, gl_PrimitiveID, 0);

		gl_TessLevelOuter[1] = (neighbour.x == -1) ? local_tess_level : max(local_tess_level, patch_tess_levels[neighbour.x + patch_offset]);

		gl_TessLevelOuter[3] = (neighbour.y == -1) ? local_tess_level : max(local_tess_level, patch_tess_levels[neighbour.y+ patch_offset]);

		gl_TessLevelOuter[0] = (neighbour.z == -1) ? local_tess_level : max(local_tess_level, patch_tess_levels[neighbour.z+ patch_offset]);

		gl_TessLevelOuter[2] = (neighbour.w == -1) ? local_tess_level : max(local_tess_level, patch_tess_levels[neighbour.w+ patch_offset]);
		
  //      gl_TessLevelInner[0] = TessLevelInner;
		//gl_TessLevelInner[1] = TessLevelInner;
  //      gl_TessLevelOuter[0] = TessLevelOuter;
  //      gl_TessLevelOuter[1] = TessLevelOuter;
		//gl_TessLevelOuter[2] = TessLevelOuter;
		//gl_TessLevelOuter[3] = TessLevelOuter;

    }
}

