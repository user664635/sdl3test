#version 310 es
layout(location = 0) in highp vec4 attr;

highp out vec2 cord;
void main() {
  gl_Position = vec4(attr.xy, -.1, 1);
  cord = attr.zw;
}
