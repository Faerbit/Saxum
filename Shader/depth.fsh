#version 150

in vec4 fragPosition;

out float fragmentDepth;

uniform vec3 cameraPosition;

void main() {
    fragmentDepth = length(vec3(fragPosition) - cameraPosition);
}
