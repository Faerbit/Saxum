#version 150

out float fragmentDepth;

void main() {
    fragmentDepth = gl_FragCoord.z;
}
