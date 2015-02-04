#version 150

in vec4 fragPosition;

uniform float farPlane;

out float gl_FragDepth;

void main() {
    gl_FragDepth = length(fragPosition)/farPlane;
}
