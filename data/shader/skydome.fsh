#version 150

in vec2 vTexCoord;
in vec4 fragPosition;
in vec4 sunPosition;

out vec4 oColor;

uniform sampler2D uTexture;
uniform sampler2D nightTexture;
uniform float farPlane;
uniform vec4 fogColor;
uniform vec3 cameraCenter;
uniform vec3 sunColor;
uniform vec3 directionalVector;

const float sunSize = 40.0;

void main() {
    vec4 textureColor = vec4(0.0, 0.0, 0.0, 1.0);
    float sunAngle = -dot(normalize(directionalVector), vec3(0.0, 1.0, 0.0));
    vec4 dayColor = texture(uTexture, vTexCoord);
    if (sunAngle >= 0.0) {
        textureColor = mix(dayColor, texture(nightTexture, vTexCoord), sunAngle);
        textureColor = mix(vec4(0.0, 0.0, 0.0, 1.0), textureColor, 1.0 - sunAngle);
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
        vec4 color = mix(textureColor, vec4(sunColor, sunIntensity), sunIntensity);
        oColor = mix(color, fogColor, fogFactor);
    }
    else {
        oColor = mix(textureColor, fogColor, fogFactor);
    }
}
