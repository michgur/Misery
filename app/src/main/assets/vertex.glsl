#version 300 es

precision mediump float;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

out vec2 tex;
out vec3 norm;
out vec3 pos;
out vec3 color;

uniform mat4 mvp;
uniform mat4 model;

void main() {
    gl_Position = mvp * vec4(position, 1.0);

    tex = texCoord;
    norm = normalize((model * vec4(normal, 0.0)).xyz);
    pos = (model * vec4(position, 1.0)).xyz;

    color = vec3(0.95);
}
