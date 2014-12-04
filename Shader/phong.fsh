#version 150

in vec3 vNormal;
in vec2 vTexCoord;
in vec4 fragPosition;
in vec4 shadowCoord;

out vec4 oColor;

uniform sampler2D uTexture;
uniform sampler2DShadow shadowMap;
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
uniform float fogEnd;
uniform vec4 fogColor;
uniform vec3 cameraCenter;

void main()
{   
    vec3 ambientColor = ambientFactor * ambientColor;
    vec3 diffuseColor = vec3(0.0, 0.0, 0.0);
    vec3 specularColor = vec3(0.0, 0.0, 0.0);


    // direction lighting
    if(length(directionalLightVector)>0.0f) {
        vec3 directionalVector = normalize(directionalLightVector);
        diffuseColor += clamp(dot(normalize(vNormal), directionalVector)
        *diffuseFactor*directionalIntensity*directionalColor, 0.0, 1.0);
        vec3 cameraVector = normalize(camera - vec3(fragPosition));
        specularColor += clamp(pow((dot((cameraVector+directionalVector),normalize(vNormal))/(length(cameraVector+directionalVector)*length(normalize(vNormal)))),shininess), 0.0, 1.0)
        *specularFactor*directionalIntensity*directionalColor;
    }

    // point lights
    for(int i = 0; i<lightCount; i++) {
        float distance = distance(lightSources[i], vec3(fragPosition));
        // only take lights into account with meaningful contribution
        if (distance > 0.001f) {
            vec3 lightVector = normalize(lightSources[i]-vec3(fragPosition));
            float intensity = clamp(exp(-(1/lightIntensities[i])*distance), 0.0, 1.0);
            diffuseColor += clamp(dot(normalize(vNormal), lightVector)
            *diffuseFactor*intensity*lightColors[i], 0.0, 1.0);
            vec3 cameraVector = normalize(camera - vec3(fragPosition));
            specularColor += clamp(pow((dot((cameraVector+lightVector),normalize(vNormal))/(length(cameraVector+lightVector)*length(normalize(vNormal)))),shininess), 0.0, 1.0)
            *specularFactor*intensity*lightColors[i];
        }
    }

    // shadows 
    float bias = 0.001*tan(acos(clamp(dot(vNormal, -directionalLightVector), 0.0, 1.0)));
    bias = clamp(bias, 0.0, 0.01);
    vec3 biasedShadowCoord = vec3(shadowCoord);
    biasedShadowCoord.z = shadowCoord.z - bias;
    float visibility = 1.0;
    if (shadowCoord.x > 0.0 && shadowCoord.x < 1.0) {
        if (shadowCoord.y > 0.0 && shadowCoord.y < 1.0) {
            visibility = texture(shadowMap, biasedShadowCoord);
            //if (texture(shadowMap, vec3(shadowCoord), bias) < shadowCoord.z) {
                //visibility = 0.5;
            //}
        }
    }

    specularColor *= visibility;
    diffuseColor  *= visibility;

    vec3 finalColor = specularColor + diffuseColor + ambientColor;
    float distanceCameraCenter = distance(cameraCenter, vec3(fragPosition));
    float fogFactor = clamp((1.0 - (fogEnd-distanceCameraCenter)/30.0), 0.0, 1.0);
    fogFactor *= clamp((1.0-((fragPosition.y-8.0)/30.0)), 0.0, 1.0);

    vec4 texture = texture(uTexture, vTexCoord).rgba;
    oColor = vec4(finalColor, 1.0f)*texture;
    oColor = mix(oColor, fogColor, fogFactor);
}
