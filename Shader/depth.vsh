#version 150

in vec3 aPosition;
in vec3 aNormal;
in vec3 aTexcoord;

out vec4 fragPosition;

uniform mat4 modelMatrix;
uniform mat4 modelViewProjectionMatrix;

void main() {
    fragPosition = modelMatrix * vec4(aPosition, 1.0);
    gl_Position = modelViewProjectionMatrix * vec4(aPosition, 1.0);
}
