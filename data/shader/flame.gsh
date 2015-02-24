#version 150

uniform mat4 viewProjectionMatrix;
uniform float time;

layout(points) in;
layout(triangle_strip, max_vertices = 3) out;

void main() {
    gl_Position = gl_in[0].gl_Position + viewProjectionMatrix * vec4(-0.5, 0.0, 0.0, 0.0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + viewProjectionMatrix * vec4(0.5, 0.0, 0.0, 0.0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + viewProjectionMatrix * vec4(0.0, 2.0 + sin(time), 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}
