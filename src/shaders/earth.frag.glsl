#version 450

layout (location = 0) out vec4 o_colour;

layout (location = 0) in vec2 v_tex;

layout (binding = 1) uniform sampler2D u_texture;

void main() {
  o_colour = texture(u_texture, v_tex);
}
