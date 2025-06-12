#version 420



layout(location=0) in vec4 in_Position;
layout(location=1) in vec4 in_Color;
layout(location=2) in vec2 in_TexCoord;
// layout(location=3) in vec4 in_Du;
// layout(location=4) in vec4 in_Dv;

out vec4 vColor;
out vec4 vPosition;
out vec2 vTexCoord;
// out vec4 vDu;
// out vec4 vDv;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

void main(void)
{
	//vec4 preProject = ModelViewMatrix * in_Position;
	//vec4 projectedPosition = ProjectionMatrix * preProject;
	// vPosition = in_Position;
	vPosition = vec4(in_Position.xyz * in_Position.w, in_Position.w);
	vColor = in_Color;
	vTexCoord = in_TexCoord;
	// vDu = in_Du;
	// vDv = in_Dv;
}
