#version 150

in vec4 fragPosition;

out float fragmentDepth;

void main() {
    fragmentDepth = fragPosition.z;
}
