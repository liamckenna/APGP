//paraboloid.fs.glsl
#version 450 core
in vec3 vPos;
out float fragDepth;
void main() {
  fragDepth = length(vPos);
}
