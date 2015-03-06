#version 150

in vec2 vTexCoord;

uniform sampler2D flame_fbo;
uniform sampler2D light_fbo;

out vec4 oColor;

void main() {
    vec4 flameColor = texture(flame_fbo, vTexCoord);
    if (flameColor == vec4(0.0, 0.0, 0.0, 1.0)) {
        oColor = texture(light_fbo, vTexCoord);
    }
    else {
        oColor = mix(flameColor, texture(light_fbo, vTexCoord), 0.4);
    }
}
