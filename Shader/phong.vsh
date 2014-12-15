#version 150

uniform mat4 modelMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 shadowMVP_near;
uniform mat4 shadowMVP_middle;
uniform mat4 shadowMVP_far;

in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoord;

out vec3 vNormal;
out vec2 vTexCoord;
out vec4 fragPosition;
out vec4 shadowCoord_near;
out vec4 shadowCoord_middle;
out vec4 shadowCoord_far;

void main()
{
    fragPosition = modelMatrix * vec4(aPosition, 1.0);
    vNormal     = inverse(transpose(mat3(modelMatrix))) * aNormal;
    vTexCoord   = aTexCoord;
    shadowCoord_near = shadowMVP_near * modelMatrix * vec4(aPosition, 1.0);
    shadowCoord_middle = shadowMVP_middle * modelMatrix * vec4(aPosition, 1.0);
    shadowCoord_far = shadowMVP_far * modelMatrix * vec4(aPosition, 1.0);
    gl_Position = modelViewProjectionMatrix * vec4(aPosition, 1.0);
}
