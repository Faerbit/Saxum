#version 150

in vec3 vNormal;
in vec2 vTexCoord;

out vec4 oColor;

uniform sampler2D uTexture;
uniform int lightCount;
uniform vec3 lightSources[128];
uniform vec3 ambientIntensity;
uniform float ambientFactor;
uniform vec3 diffuseIntensity;
uniform float diffuseFactor;

void main()
{   
    vec3 ambientColor = ambientFactor * ambientIntensity;
    vec3 diffuseColor = vec3(0.0, 0.0, 0.0);
    for(int i = 0; i<lightCount; i++) {
        diffuseColor += dot(normalize(vNormal), normalize(lightSources[i]))*diffuseFactor*diffuseIntensity;
    }
    vec3 finalColor = diffuseColor + ambientColor;

    vec3 texture = texture(uTexture, vTexCoord).rgb;
    oColor = vec4(finalColor*texture, 1.0 );
}
