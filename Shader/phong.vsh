#version 150

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoord;

out vec3 vNormal;
out vec2 vTexCoord;
out vec4 fragPosition;

void main()
{
    fragPosition = modelMatrix * vec4(aPosition, 1.0);
    vNormal     = inverse(transpose(mat3(modelMatrix))) * aNormal;
    vTexCoord   = aTexCoord;
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPosition, 1.0);
}
