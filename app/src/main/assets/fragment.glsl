#version 300 es

precision mediump float;

in vec3 pos;
in vec2 tex;
in vec3 norm;
in vec3 color;

out vec4 fragmentColor;

uniform sampler2D diffuse;

void main() { fragmentColor = texture(diffuse, tex); }
