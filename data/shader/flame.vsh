#version 150

uniform mat4 viewProjectionMatrix;

in vec3 aPosition;

void main () {
    gl_Position = viewProjectionMatrix * vec4(aPosition, 1.0);
}
