#version 150

uniform mat4 modelMatrix;
uniform mat4 lightingViewProjectionMatrix;
uniform mat4 shadowMVP;

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
    shadowCoord = shadowMVP * modelMatrix * vec4(aPosition, 1.0);
    gl_Position = lightingViewProjectionMatrix * modelMatrix * vec4(aPosition, 1.0);
}
