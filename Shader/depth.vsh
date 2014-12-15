#version 150

in vec3 aPosition;
in vec3 aNormal;
in vec3 aTexcoord;

uniform mat4 modelViewProjectionMatrix;

void main() {
    gl_Position = modelViewProjectionMatrix * vec4(aPosition, 1.0);
}
