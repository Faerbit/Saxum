#version 150

in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoord;

uniform mat4 modelMatrix;
uniform mat4 modelViewProjectionMatrix;

out vec2 vTexCoord;
out vec4 fragPosition;

void main() {
    fragPosition = modelMatrix * vec4(aPosition, 1.0);
    vTexCoord = aTexCoord;
    gl_Position = modelViewProjectionMatrix * vec4(aPosition, 1.0);
}
