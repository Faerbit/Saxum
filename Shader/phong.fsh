#version 150

in vec3 vNormal;
in vec2 vTexCoord;
in vec4 fragPosition;

out vec4 oColor;

uniform sampler2D uTexture;
uniform int lightCount;
uniform vec3 lightSources[128];
uniform vec3 ambientColor;
uniform float ambientFactor;
uniform vec3 lightColors[128];
uniform float lightIntensities[128];

void main()
{   
    vec3 ambientColor = ambientFactor * ambientColor;
    vec3 diffuseColor = vec3(0.0, 0.0, 0.0);
    for(int i = 0; i<lightCount; i++) {
        float distance = distance(lightSources[i], vec3(fragPosition));
        float diffuseFactor = (1.0*lightIntensities[i])/(distance);
        diffuseColor += dot(normalize(vNormal), normalize(lightSources[i]))
        *diffuseFactor*lightColors[i];
    }
    vec3 finalColor = diffuseColor + ambientColor;

    vec3 texture = texture(uTexture, vTexCoord).rgb;
    oColor = vec4(finalColor*texture, 1.0 );
}
