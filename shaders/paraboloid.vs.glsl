//paraboloid.vs.glsl
#version 450 core
layout(location=0) in vec3 inPos;
uniform vec3 lightPos;
uniform int z_direction;
out vec3 vPos;
void main() {
	if (z_direction == 0) {
		vPos = inPos - lightPos;
	} else if (z_direction == 1) {
		vPos = vec3(vPos.xy, -vPos.z);
	}
	float m = 2.0 / (length(vPos) + vPos.z);
	gl_Position = vec4(vPos.xy * m, 0.0, 1.0);


}
