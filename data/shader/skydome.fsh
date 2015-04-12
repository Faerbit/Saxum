#version 150

in vec2 vTexCoord;
in vec4 fragPosition;
in vec4 sunPosition;

out vec4 oColor;

uniform sampler2D dayTexture;
uniform sampler2D nightTexture;
uniform float farPlane;
uniform vec4 fogColorDay;
uniform vec4 fogColorRise;
uniform vec4 fogColorNight;
uniform vec3 cameraCenter;
uniform vec3 sunColor;
uniform vec3 directionalVector;

const float sunSize = 20.0;

vec4 fogColor(float dot) {
    float riseFactor = 0.0;
    float dayFactor = 0.0;

    if(dot<-0.52) {
        riseFactor = 0.0;
    }
    else if (dot>0.52) {
        riseFactor = 0.0;
    }
    else {
        riseFactor = cos(3*dot);
    }

    if(dot<0.0) {
        dayFactor = 0.0;
    }
    else if(dot>1.0) {
        dayFactor = 1.0;
    }
    else {
        dayFactor = sin(dot);
    }
    if (dot <0.0) {
        return mix(fogColorNight, fogColorRise, riseFactor);
    }
    else {
        return mix(fogColorRise, fogColorDay, dayFactor);
    }

}

vec3 sunColorFunc(float dot) {
    float riseFactor = 0.0;
    if(dot<-0.79) {
        riseFactor = 0.0;
    }
    else if (dot>0.876) {
        riseFactor = 0.0;
    }
    else {
        riseFactor = cos(2*dot);
    }
    return mix(sunColor, vec3(fogColorRise)*1.4, riseFactor);
}

void main() {
    vec4 textureColor = vec4(0.0, 0.0, 0.0, 1.0);
    float sunAngle = -dot(normalize(directionalVector), vec3(0.0, 1.0, 0.0));
    vec4 dayColor = texture(dayTexture, vTexCoord);
    if (sunAngle >= 0.0) {
        textureColor = mix(dayColor, texture(nightTexture, vTexCoord), sunAngle);
    }
    else {
        textureColor = dayColor;
    }
    float distanceToSun = length(sunPosition - fragPosition);
    float distanceCameraCenter = distance(cameraCenter, vec3(fragPosition));
    float fogFactor = clamp((1.0 - ((farPlane - 35.0) -distanceCameraCenter)/30.0), 0.0, 1.0);
    fogFactor *= clamp((1.0-((fragPosition.y-40.0)/30.0)), 0.0, 1.0);
    if (distanceToSun < sunSize) {
        float sunIntensity = clamp(0.3*exp(1/(distanceToSun/sunSize))-exp(1.0)*0.3, 0.0, 1.0);
        vec4 fogcolor = mix(textureColor, fogColor(-sunAngle), fogFactor);
        oColor = mix(fogcolor, vec4(sunColorFunc(sunAngle), sunIntensity), sunIntensity*sin(2*(-sunAngle)));
    }
    else {
        oColor = mix(textureColor, fogColor(-sunAngle), fogFactor);
    }
}
