#version 310 es

layout(location = 0) in highp vec4 pos_in;
layout(location = 1) in highp vec4 color_in;

layout(location = 0) uniform highp float scale;
layout(location = 1) uniform highp mat4 trans;

out highp vec4 color;
void main() {
  highp vec4 p = trans * pos_in;
  gl_Position = vec4(p.xy * scale , -p.z - 1. / 64., -p.z);
  color = color_in;
}

