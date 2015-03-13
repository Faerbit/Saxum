#version 150

uniform mat4 viewProjectionMatrix;

in vec3 aPosition;
in vec3 aColor;

out vec3 vColor;

void main() {
    vColor = aColor;
    gl_Position = viewProjectionMatrix * vec4(aPosition, 1.0);
}
