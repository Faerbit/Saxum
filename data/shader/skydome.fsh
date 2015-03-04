#version 150

in vec2 vTexCoord;
in vec4 fragPosition;

out vec4 oColor;

uniform sampler2D uTexture;
uniform float farPlane;
uniform vec4 fogColor;
uniform vec3 cameraCenter;

void main() {
    float distanceCameraCenter = distance(cameraCenter, vec3(fragPosition));
    float fogFactor = clamp((1.0 - ((farPlane - 35.0) -distanceCameraCenter)/30.0), 0.0, 1.0);
    fogFactor *= clamp((1.0-((fragPosition.y-40.0)/30.0)), 0.0, 1.0);
    oColor = mix(texture(uTexture, vTexCoord), fogColor, fogFactor);
}
