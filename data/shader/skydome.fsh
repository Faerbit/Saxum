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
uniform vec3 directionalVector;
uniform float skydomeSize;

const float sunSize = 20.0;
const float starSize = 1.0;
const vec4 starColor = vec4(1.0, 1.0, 0.9, 1.0);

const int starCount = 2;
vec3 starPositions[starCount] = vec3[](
    vec3(-0.3102524288591748, 0.9505037096381865, -0.016915328877346533),
    vec3(-0.14085574439428544, 0.9519584459035886, -0.27190950065041153)
);

float starSizes[starCount] = float[] (
    float(0.5),
    float(1.2)
);

void main() {
    vec4 textureColor = vec4(0.0, 0.0, 0.0, 1.0);
    float sunAngle = -dot(normalize(directionalVector), vec3(0.0, 1.0, 0.0));
    vec4 dayColor = texture(uTexture, vTexCoord);
    if (sunAngle >= 0.0) {
        vec4 nightColor = vec4(0.0, 0.0, 0.0, 1.0);
        for(int i = 0; i<starCount; i++) {
            vec3 starPosition = (starPositions[i] * skydomeSize) + vec3(cameraCenter.x, 0.0, cameraCenter.z);
            float distanceToStar = distance(starPosition, vec3(fragPosition));
            float thisStarSize = starSize * starSizes[i];
            if (distanceToStar < thisStarSize) {
                float starIntensity = clamp(0.3*exp(1/(distanceToStar/thisStarSize))-exp(1.0)*0.3, 0.0, 1.0);
                nightColor = mix(nightColor, starColor, starIntensity);
            }
        }
        textureColor = mix(dayColor, nightColor, sunAngle);
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
