#version 150

in vec3 fColor;

out vec4 oColor;

uniform bool withColor;


void main() {
    if (withColor) {
        oColor = vec4(fColor, 1.0);
    }
    else {
        oColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}
