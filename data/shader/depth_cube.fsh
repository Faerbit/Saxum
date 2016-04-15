#version 150

in vec4 fragPosition;

uniform float farPlane;

void main() {
    float nearPlane = 0.1;
    float A = -(farPlane+nearPlane)/(farPlane-nearPlane);
    float B = -2*(farPlane*nearPlane)/(farPlane - nearPlane);
    float value = 0.5*(-A*length(fragPosition) + B)/length(fragPosition) + 0.5;
    gl_FragDepth = value;
}
