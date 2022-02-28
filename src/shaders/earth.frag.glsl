#version 450

layout (location = 0) out vec4 o_colour;

layout (location = 0) in vec2 v_tex;
layout (location = 1) in vec3 v_norm;

layout (binding = 2) uniform sampler2D u_diffuse_texture;
layout (binding = 3) uniform sampler2D u_night_texture;

void main() {
  const vec3 sun_dir = normalize(vec3(0, -0.2, 1));
  float darkness = dot(v_norm, sun_dir);
  o_colour = mix(texture(u_night_texture, v_tex), texture(u_diffuse_texture, v_tex), darkness);
}
