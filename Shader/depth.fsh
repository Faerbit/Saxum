#version 150

in vec4 fragPosition;

out float gl_FragDepth;

void main() {
    gl_FragDepth = fragPosition.z;
    //gl_FragDepth = 0.0f;
}
