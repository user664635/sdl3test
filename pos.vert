#version 460 core

layout(location = 0) in vec4 pos_in;
layout(location = 1) in vec4 color_in;

layout(location = 0) uniform float scale;
//layout(location = 1) uniform mat4 trans;

out vec4 color;
void main() {
  vec4 p = pos_in;
  gl_Position = vec4(p.xy * scale , -p.z - 1 / 64., -p.z);
  color = color_in;
}

