#version 150

in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoord;

uniform mat4 modelMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform float skydomeSize;
uniform vec3 directionalVector;
uniform vec3 cameraCenter;

out vec2 vTexCoord;
out vec4 fragPosition;
out vec4 sunPosition;

void main() {
    fragPosition = modelMatrix * vec4(aPosition, 1.0);
    vTexCoord = aTexCoord;
    sunPosition = (normalize(vec4(directionalVector, 0.0)) * skydomeSize) + vec4(cameraCenter.x, 0, cameraCenter.z, 1.0);
    gl_Position = modelViewProjectionMatrix * vec4(aPosition, 1.0);
}
