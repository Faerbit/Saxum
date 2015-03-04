#version 150

in vec2 vTexCoord;

uniform sampler2D light_fbo;
uniform int windowSizeX;
uniform int windowSizeY;

out vec4 oColor;

const float lookup_offset = 4.0;

void main() {
    vec4 sum = vec4(0.0);
    for(float i = -lookup_offset; i<=lookup_offset; i+=1.0) {
        for(float j = -lookup_offset; j<=lookup_offset; j+=1.0) {
            sum += texture(light_fbo, vec2(vTexCoord.x + i * 1.0/(windowSizeX/2.0), vTexCoord.y + j * 1.0/(windowSizeY/2.0)))/pow(lookup_offset*2+1, 2.0);
        }
    }
    oColor = sum;
}
