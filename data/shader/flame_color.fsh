#version 150

in vec2 vTexCoord;

uniform sampler2D flame_fbo;

out vec4 oColor;

void main() {
    oColor = texture(flame_fbo, vTexCoord);
}
