#version 150

in vec2 vTexCoord;
in vec4 fragPosition;
in vec4 sunPosition;

out vec4 oColor;

uniform sampler2D uTexture;
uniform float farPlane;
uniform vec4 fogColor;
uniform vec3 cameraCenter;
uniform vec3 sunColor;

const float sunSize = 40.0;

void main() {
    float distanceToSun = length(sunPosition - fragPosition);
    float distanceCameraCenter = distance(cameraCenter, vec3(fragPosition));
    float fogFactor = clamp((1.0 - ((farPlane - 35.0) -distanceCameraCenter)/30.0), 0.0, 1.0);
    fogFactor *= clamp((1.0-((fragPosition.y-40.0)/30.0)), 0.0, 1.0);
    if (distanceToSun < sunSize) {
        float sunIntensity = clamp(0.3*exp(1/(distanceToSun/sunSize))-exp(1.0)*0.3, 0.0, 1.0);
        vec4 color = mix(vec4(texture(uTexture, vTexCoord)), vec4(sunColor, sunIntensity), sunIntensity);
        oColor = mix(color, fogColor, fogFactor);
    }
    else {
        oColor = mix(texture(uTexture, vTexCoord), fogColor, fogFactor);
    }
}
