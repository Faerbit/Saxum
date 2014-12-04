#version 150

out vec4 fragmentDepth;

void main() {
    fragmentDepth.r = gl_FragCoord.z;
    fragmentDepth.g = gl_FragCoord.z;
    fragmentDepth.b = gl_FragCoord.z;
    fragmentDepth.a = 1.0;
}
