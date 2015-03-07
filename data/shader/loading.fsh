#version 150

in vec2 vTexCoord;

uniform float time;
uniform sampler2D screen;
uniform sampler2D screenContinue;

out vec4 oColor;

void main() {
    vec4 screenColor = texture(screen, vTexCoord);
    vec4 screenContinueColor = texture(screenContinue, vTexCoord);
    oColor = mix(screenColor, screenContinueColor, pow(sin(time), 2));
}
