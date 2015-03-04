#version 150

in vec2 vTexCoord;

uniform sampler2D light_fbo;
uniform int windowSizeX;
uniform int windowSizeY;
uniform bool verticalPass;

out vec4 oColor;

//const float lookup_offset = 4.0;
const float stretching = 2.0;

void main() {
    vec4 sum = vec4(0.0);
    if (verticalPass){
        float blurSize = 1.0/(float(windowSizeY)/(stretching));
        sum += texture(light_fbo, vec2(vTexCoord.x - 4.0*blurSize, vTexCoord.y)) * 0.05;
        sum += texture(light_fbo, vec2(vTexCoord.x - 3.0*blurSize, vTexCoord.y)) * 0.09;
        sum += texture(light_fbo, vec2(vTexCoord.x - 2.0*blurSize, vTexCoord.y)) * 0.12;
        sum += texture(light_fbo, vec2(vTexCoord.x - blurSize, vTexCoord.y)) * 0.15;
        sum += texture(light_fbo, vec2(vTexCoord.x, vTexCoord.y)) * 0.16;
        sum += texture(light_fbo, vec2(vTexCoord.x + blurSize, vTexCoord.y)) * 0.15;
        sum += texture(light_fbo, vec2(vTexCoord.x + 2.0*blurSize, vTexCoord.y)) * 0.12;
        sum += texture(light_fbo, vec2(vTexCoord.x + 3.0*blurSize, vTexCoord.y)) * 0.09;
        sum += texture(light_fbo, vec2(vTexCoord.x + 4.0*blurSize, vTexCoord.y)) * 0.05;
    }
    else {
        float blurSize = 1.0/(float(windowSizeX)/stretching);
        sum += texture(light_fbo, vec2(vTexCoord.x, vTexCoord.y - 4.0*blurSize)) * 0.05;
        sum += texture(light_fbo, vec2(vTexCoord.x, vTexCoord.y - 3.0*blurSize)) * 0.09;
        sum += texture(light_fbo, vec2(vTexCoord.x, vTexCoord.y - 2.0*blurSize)) * 0.12;
        sum += texture(light_fbo, vec2(vTexCoord.x, vTexCoord.y - blurSize)) * 0.15;
        sum += texture(light_fbo, vec2(vTexCoord.x, vTexCoord.y)) * 0.16;
        sum += texture(light_fbo, vec2(vTexCoord.x, vTexCoord.y + blurSize)) * 0.15;
        sum += texture(light_fbo, vec2(vTexCoord.x, vTexCoord.y + 2.0*blurSize)) * 0.12;
        sum += texture(light_fbo, vec2(vTexCoord.x, vTexCoord.y + 3.0*blurSize)) * 0.09;
        sum += texture(light_fbo, vec2(vTexCoord.x, vTexCoord.y + 4.0*blurSize)) * 0.05;
    }
    oColor = sum;
}
