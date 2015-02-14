#version 150

in vec3 aPosition;
in vec3 aNormal;
in vec3 aTexcoord;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;

out vec4 fragPosition;

void main() {
    fragPosition = modelViewMatrix * vec4(aPosition, 1.0);
    gl_Position = modelViewProjectionMatrix * vec4(aPosition, 1.0);
}
