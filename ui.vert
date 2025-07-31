#version 310 es

layout(location = 0) in highp vec4 attr_in;
layout(location = 1) in highp vec4 cord_in;

highp out vec4 attr;
highp out vec4 cord;
void main() {
  attr = attr_in;
  cord = cord_in;
}
