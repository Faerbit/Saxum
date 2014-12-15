#version 150

in vec3 vNormal;
in vec2 vTexCoord;
in vec4 fragPosition;
in vec4 shadowCoord_near;
in vec4 shadowCoord_middle;
in vec4 shadowCoord_far;

out vec4 oColor;

uniform sampler2D uTexture;
uniform sampler2DShadow shadowMap_near;
uniform sampler2DShadow shadowMap_middle;
uniform sampler2DShadow shadowMap_far;
uniform samplerCubeShadow shadowMap_cube;
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
uniform vec3 lightSources[32];
uniform vec3 lightColors[32];
uniform float lightIntensities[32];
uniform float fogEnd;
uniform vec4 fogColor;
uniform vec3 cameraCenter;

vec2 poissonDisk[16] = vec2[](
   vec2( -0.94201624, -0.39906216 ),
   vec2( 0.94558609, -0.76890725 ),
   vec2( -0.094184101, -0.92938870 ),
   vec2( 0.34495938, 0.29387760 ),
   vec2( -0.91588581, 0.45771432 ),
   vec2( -0.81544232, -0.87912464 ),
   vec2( -0.38277543, 0.27676845 ),
   vec2( 0.97484398, 0.75648379 ),
   vec2( 0.44323325, -0.97511554 ),
   vec2( 0.53742981, -0.47373420 ),
   vec2( -0.26496911, -0.41893023 ),
   vec2( 0.79197514, 0.19090188 ),
   vec2( -0.24188840, 0.99706507 ),
   vec2( -0.81409955, 0.91437590 ),
   vec2( 0.19984126, 0.78641367 ),
   vec2( 0.14383161, -0.14100790 )
);

float sampleDirectionalShadow(sampler2DShadow shadowMap, vec4 shadowCoord) {
    float visibility = 1.0;
    float bias = 0.001*tan(acos(clamp(dot(vNormal, -directionalLightVector), 0.0, 1.0)));
    bias = clamp(bias, 0.0, 0.01);
    for (int i=0; i<4; i++) {
        visibility -= directionalIntensity/16*(1.0-texture(shadowMap, vec3(shadowCoord.xy + poissonDisk[i]/700.0, (shadowCoord.z - bias)/shadowCoord.w)));
    }
    if (visibility == 1.0-(directionalIntensity/16)*4)
    {
        visibility = 1.0-directionalIntensity;
    }
    else if (visibility != 1.0) {
        for (int i=0; i<12; i++) {
            visibility -= directionalIntensity/16*(1.0-texture(shadowMap, vec3(shadowCoord.xy + poissonDisk[i]/700.0, (shadowCoord.z - bias)/shadowCoord.w)));
        }
    }
    return visibility;
}

float samplePointShadow(samplerCubeShadow shadowMap, vec3 lightDirection) {
    float bias = 0.005;
    return texture(shadowMap, vec4(lightDirection.xyz , length(lightDirection) - bias));
}

float distanceToBorder(vec2 vector) {
    float xDistance = min(vector.x, 1.0-vector.x);
    float yDistance = min(vector.y, 1.0-vector.y);
    return min(xDistance, yDistance);
}

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
    float visibility = 1.0;
    for(int i = 0; i<lightCount; i++) {
        vec3 lightDirection = vec3(fragPosition) - lightSources[i];
        float distance = length(lightDirection);
        // only take lights into account with meaningful contribution
        if (distance > 0.001f) {
            vec3 lightVector = normalize(lightSources[i]-vec3(fragPosition));
            float intensity = clamp(exp(-(1/lightIntensities[i])*distance), 0.0, 1.0);
            diffuseColor += clamp(dot(normalize(vNormal), lightVector)
            *diffuseFactor*intensity*lightColors[i], 0.0, 1.0);
            vec3 cameraVector = normalize(camera - vec3(fragPosition));
            specularColor += clamp(pow((dot((cameraVector+lightVector),normalize(vNormal))/(length(cameraVector+lightVector)*length(normalize(vNormal)))),shininess), 0.0, 1.0)
            *specularFactor*intensity*lightColors[i];
            visibility = samplePointShadow(shadowMap_cube, lightDirection);
        }
    }

    // shadows 
    if (distanceToBorder(shadowCoord_middle.xy) <= 0.5 && distanceToBorder(shadowCoord_middle.xy) > 0.0) {
        if (distanceToBorder(shadowCoord_near.xy) <= 0.5 && distanceToBorder(shadowCoord_near.xy) > 0.0) {
                visibility *= sampleDirectionalShadow(shadowMap_near, shadowCoord_near);
        }
        else {
            visibility *= sampleDirectionalShadow(shadowMap_middle, shadowCoord_middle);
        }
    }
    else {
        visibility *= sampleDirectionalShadow(shadowMap_far, shadowCoord_far);
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
