#version 150

uniform mat4 viewProjectionMatrix;
uniform vec2 skew;

in vec3 aPosition;
in vec3 aColor;

out vec3 Color;

const float end = 0.8;

void main () {
    Color = aColor;
    vec3 position = aPosition + vec3(-sin(skew.y)*end, 0.0, sin(skew.x)*end);
    gl_Position = viewProjectionMatrix * vec4(position, 1.0);
}
