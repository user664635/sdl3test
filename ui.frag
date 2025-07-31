#version 310 es

in highp vec4 c;
in highp vec2 p;

out highp vec4 color;

layout(location = 0) uniform uint indx;
layout(location = 1) uniform sampler2D tex[2];

void main() {
  color = c * texture(tex[indx], p);
}

