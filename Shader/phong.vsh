#version 150

uniform mat4 modelMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 shadowMVPs[5];

in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoord;

out vec3 vNormal;
out vec2 vTexCoord;
out vec4 fragPosition;
out vec4 shadowCoord;

void main()
{
    fragPosition = modelMatrix * vec4(aPosition, 1.0);
    vNormal     = inverse(transpose(mat3(modelMatrix))) * aNormal;
    vTexCoord   = aTexCoord;
    shadowCoord = shadowMVPs[0] * vec4(aPosition, 1.0);
    gl_Position = modelViewProjectionMatrix * vec4(aPosition, 1.0);
}
