#version 310 es

in highp vec2 cord;
out highp vec4 color;

layout(location = 0) uniform uint indx;
layout(location = 1) uniform sampler2D tex[2];
void main() {
  color = vec4(texture(tex[indx], cord).xyz, .5);
}

