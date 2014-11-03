#version 150

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

in vec3 aNormal;
in vec3 aPosition;
in vec2 aTexCoord;

out vec3 vNormal;
out vec2 vTexCoord;
out vec4 fragPosition;

void main()
{
    fragPosition = uViewMatrix * vec4(aPosition, 1.0);
    vNormal     = inverse(transpose(mat3(uViewMatrix))) * aNormal;
    vTexCoord   = aTexCoord;
    gl_Position = uProjectionMatrix * uViewMatrix * vec4(aPosition, 1.0);
}
