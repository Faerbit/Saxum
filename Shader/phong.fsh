#version 150

in vec3 vNormal;
in vec2 vTexCoord;
in vec4 fragPosition;

out vec4 oColor;

uniform sampler2D uTexture;
uniform vec3 ambientColor;
uniform float ambientFactor;
uniform float diffuseFactor;
uniform float specularFactor;
uniform vec3 camera;
uniform float shininess;
uniform int lightCount;
uniform vec3 directionalLightVector;
uniform vec3 directionalColor;
uniform float directionalIntensity;
uniform vec3 lightSources[128];
uniform vec3 lightColors[128];
uniform float lightIntensities[128];

void main()
{   
    vec3 ambientColor = ambientFactor * ambientColor;
    vec3 diffuseColor = vec3(0.0, 0.0, 0.0);
    vec3 specularColor = vec3(0.0, 0.0, 0.0);

    if(length(directionalLightVector)>0.0f) {
        vec3 directionalVector = normalize(directionalLightVector);
        diffuseColor += clamp(dot(normalize(vNormal), directionalVector)
        *diffuseFactor*directionalIntensity*directionalColor, 0.0, 1.0);
        vec3 cameraVector = normalize(camera - vec3(fragPosition));
        specularColor += clamp(pow((dot((cameraVector+directionalVector),normalize(vNormal))/(length(cameraVector+directionalVector)*length(normalize(vNormal)))),shininess), 0.0, 1.0)
        *specularFactor*directionalIntensity*directionalColor;
    }
    for(int i = 0; i<lightCount; i++) {
        float distance = distance(lightSources[i], vec3(fragPosition));
        // only take lights into account with meaningful contribution
        if (distance > 0.001f) {
            vec3 lightVector = normalize(lightSources[i]-vec3(fragPosition));
            float intensity = (lightIntensities[i])/(distance);
            diffuseColor += clamp(dot(normalize(vNormal), lightVector)
            *diffuseFactor*intensity*lightColors[i], 0.0, 1.0);
            vec3 cameraVector = normalize(camera - vec3(fragPosition));
            specularColor += clamp(pow((dot((cameraVector+lightVector),normalize(vNormal))/(length(cameraVector+lightVector)*length(normalize(vNormal)))),shininess), 0.0, 1.0)
            *specularFactor*intensity*lightColors[i];
        }
    }
    vec3 finalColor = specularColor + diffuseColor + ambientColor;

    vec4 texture = texture(uTexture, vTexCoord).rgba;
    oColor = vec4(finalColor, 1.0f)*texture;
}
