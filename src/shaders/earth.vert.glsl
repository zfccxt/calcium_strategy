#version 450

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex;
layout (location = 2) in vec3 a_norm;

layout (location = 0) out vec2 v_tex;

layout (binding = 0) uniform ViewProjectionMatrix {
  mat4 matrix;
} u_viewprojection;

void main() {
  gl_Position = u_viewprojection.matrix * vec4(a_pos, 1.0);
  v_tex = a_tex;
}
