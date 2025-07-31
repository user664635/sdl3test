#version 310 es

layout(location = 0) in highp vec3 pos_in;
layout(location = 1) in highp vec4 color_in;

layout(location = 0) uniform highp vec2 scale;
layout(location = 1) uniform highp vec3 view;
layout(location = 2) uniform highp mat4 rot;

out highp vec4 color;
void main() {
  highp vec4 p = vec4(pos_in - view, 0);
  p *= rot;
  gl_Position = vec4(p.xy * scale , -p.z - 1. / 64., -p.z);
  color = color_in;
}

