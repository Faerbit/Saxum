#version 150

in vec3 aPosition;
in vec3 aNormal;
in vec3 aTexcoord;

uniform mat4 viewProjectionMatrix;
uniform mat4 modelMatrix;

void main() {
    gl_Position = viewProjectionMatrix * modelMatrix * vec4(aPosition, 1.0);
}
