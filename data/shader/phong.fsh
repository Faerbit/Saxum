#version 150

in vec3 vNormal;
in vec2 vTexCoord;
in vec4 fragPosition;
in vec4 shadowCoord0;
in vec4 shadowCoord1;
in vec4 shadowCoord2;

out vec4 oColor;

uniform sampler2D uTexture;
uniform sampler2DShadow shadowMap_directional0;
uniform sampler2DShadow shadowMap_directional1;
uniform sampler2DShadow shadowMap_directional2;
uniform samplerCubeShadow shadowMap_cube0;
uniform samplerCubeShadow shadowMap_cube1;
uniform samplerCubeShadow shadowMap_cube2;
uniform samplerCubeShadow shadowMap_cube3;
uniform samplerCubeShadow shadowMap_cube4;
uniform samplerCubeShadow shadowMap_cube5;
uniform samplerCubeShadow shadowMap_cube6;
uniform samplerCubeShadow shadowMap_cube7;
uniform samplerCubeShadow shadowMap_cube8;
uniform samplerCubeShadow shadowMap_cube9;
uniform vec3 ambientColor;
uniform float ambientFactor;
uniform float diffuseFactor;
uniform float specularFactor;
uniform vec3 camera;
uniform float shininess;
uniform int lightCount;
uniform int maxShadowRenderCount;
uniform vec3 directionalLightVector;
uniform vec3 directionalColor;
uniform float directionalIntensity;
uniform vec3 lightSources[32];
uniform vec3 lightColors[32];
uniform float lightIntensities[32];
uniform bool isFlame[32];
uniform float farPlane;
uniform vec4 fogColorDay;
uniform vec4 fogColorRise;
uniform vec4 fogColorNight;
uniform vec3 cameraCenter;
uniform bool movingTexture;
uniform vec2 movement;
uniform vec2 movingTextureOffset;
uniform float time;

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

float flickerFunction(int index) {
    float windPower = length(movement);
    if (windPower < 0.8) {
        windPower += 0.2;
    }
    return windPower * pow(sin((20.0/(windPower))*time + lightSources[index].x*lightSources[index].z), 2) + (1-windPower);
}

vec4 fogColor(float dot) {
    float riseFactor = 0.0;
    float dayFactor = 0.0;

    if(dot<-0.52) {
        riseFactor = 0.0;
    }
    else if (dot>0.52) {
        riseFactor = 0.0;
    }
    else {
        riseFactor = cos(3*dot);
    }

    if(dot<0.0) {
        dayFactor = 0.0;
    }
    else if(dot>1.0) {
        dayFactor = 1.0;
    }
    else {
        dayFactor = sin(dot);
    }
    if (dot <0.0) {
        return mix(fogColorNight, fogColorRise, riseFactor);
    }
    else {
        return mix(fogColorRise, fogColorDay, dayFactor);
    }

}

vec3 sunColor(float dot){
    float riseFactor = 0.0;
    if(dot<-0.79) {
        riseFactor = 0.0;
    }
    else if (dot>0.79) {
        riseFactor = 0.0;
    }
    else {
        riseFactor = cos(2*dot);
    }
    return mix(directionalColor, vec3(fogColorRise), riseFactor);
}

float sampleDirectionalShadow(sampler2DShadow shadowMap, vec4 shadowCoord, float maxBias ) {
    float visibility = 1.0;
    const float stretching = 650.0;
    float bias = 0.001*tan(acos(clamp(dot(vNormal, -directionalLightVector), 0.0, 1.0)));
    bias = clamp(bias, 0.0, maxBias);
    for (int i=0; i<4; i++) {
        visibility -= directionalIntensity/16*(1.0-texture(shadowMap, vec3(shadowCoord.xy + poissonDisk[i]/stretching, shadowCoord.z - bias)));
    }
    if (visibility == 1.0-(directionalIntensity/16)*4)
    {
        visibility = 1.0-directionalIntensity;
    }
    else if (visibility != 1.0) {
        for (int i=0; i<12; i++) {
            visibility -= directionalIntensity/16*(1.0-texture(shadowMap, vec3(shadowCoord.xy + poissonDisk[i]/stretching, shadowCoord.z - bias)));
        }
    }
    return visibility;
}

float samplePointShadow(samplerCubeShadow shadowMap, vec3 lightDirection) {
    float nearPlane = 0.1;
    float A = -(farPlane+nearPlane)/(farPlane-nearPlane);
    float B = -2*(farPlane*nearPlane)/(farPlane - nearPlane);
    float compValue = 0.5*(-A*length(lightDirection) + B)/length(lightDirection) + 0.5;
    float bias = 0.001*tan(acos(clamp(dot(vNormal, normalize(lightDirection)), 0.0, 1.0)));
    bias = clamp(bias, 0.0, 0.001);
    bias *= 1/length(lightDirection)*8;
    return texture(shadowMap, vec4(lightDirection , compValue - bias));
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
    float sunAngle = -1.0;
    if(length(directionalLightVector)>0.0f) {
        vec3 directionalVector = normalize(directionalLightVector);
        sunAngle = dot(vec3(0.0, 1.0, 0.0), directionalVector);
        if ( sunAngle > -0.6) {
            float directionalVisibility = 1.0f;
            if (distanceToBorder(shadowCoord1.xy) <= 0.5 && distanceToBorder(shadowCoord1.xy) > 0.2) {
                if (distanceToBorder(shadowCoord0.xy) <= 0.5 && distanceToBorder(shadowCoord0.xy) > 0.2) {
                    directionalVisibility = sampleDirectionalShadow(shadowMap_directional0, shadowCoord0, 0.001);
                }
                else if (distanceToBorder(shadowCoord0.xy) <= 0.5 && distanceToBorder(shadowCoord0.xy) > 0.0) {
                    float directionalVisibility0 = sampleDirectionalShadow(shadowMap_directional0, shadowCoord0, 0.001);
                    float directionalVisibility1 = sampleDirectionalShadow(shadowMap_directional1, shadowCoord1, 0.002);
                    directionalVisibility = mix(directionalVisibility0, directionalVisibility1, distanceToBorder(shadowCoord0.xy) * 5);
                }
                else {
                    directionalVisibility = sampleDirectionalShadow(shadowMap_directional1, shadowCoord1, 0.002);
                }
            }
            else if (distanceToBorder(shadowCoord1.xy) <= 0.5 && distanceToBorder(shadowCoord1.xy) > 0.0) {
                float directionalVisibility1 = sampleDirectionalShadow(shadowMap_directional1, shadowCoord1, 0.01);
                float directionalVisibility2 = sampleDirectionalShadow(shadowMap_directional2, shadowCoord2, 0.01);
                directionalVisibility = mix(directionalVisibility1, directionalVisibility2, distanceToBorder(shadowCoord1.xy) * 5);
            }
            else {
                directionalVisibility = sampleDirectionalShadow(shadowMap_directional2, shadowCoord2, 0.01);
            }
            diffuseColor += clamp(dot(normalize(vNormal), directionalVector)
            *diffuseFactor*directionalIntensity*sunColor(sunAngle), 0.0, 1.0)*directionalVisibility;
            vec3 cameraVector = normalize(camera - vec3(fragPosition));
            specularColor += clamp(pow((dot((cameraVector+directionalVector),normalize(vNormal))/
            (length(cameraVector+directionalVector)*length(normalize(vNormal)))),shininess), 0.0, 1.0)
            *specularFactor*directionalIntensity*sunColor(sunAngle)*directionalVisibility;
        }
    }

    // point lights
    float visibility = 1.0;
    for(int i = 0; i<lightCount; i++) {
        vec3 lightDirection = vec3(fragPosition) - lightSources[i];
        float distance = length(lightDirection);
        float pointVisibility = 1.0f;
        // only take lights into account with meaningful contribution
        float intensity = 0.0f;
        if (isFlame[i] == true) {
            intensity = clamp(exp(-(1/(lightIntensities[i] + flickerFunction(i)))*distance), 0.0, 1.0);
        }
        else {
            intensity = clamp(exp(-(1/lightIntensities[i])*distance), 0.0, 1.0);
        }
        if (intensity > 0.005) {
            if (i == 0 && i<maxShadowRenderCount) {
                pointVisibility = samplePointShadow(shadowMap_cube0, lightDirection);
            }
            if (i == 1 && i<maxShadowRenderCount) {
                pointVisibility = samplePointShadow(shadowMap_cube1, lightDirection);
            }
            if (i == 2 && i<maxShadowRenderCount) {
                pointVisibility = samplePointShadow(shadowMap_cube2, lightDirection);
            }
            if (i == 3 && i<maxShadowRenderCount) {
                pointVisibility = samplePointShadow(shadowMap_cube3, lightDirection);
            }
            if (i == 4 && i<maxShadowRenderCount) {
                pointVisibility = samplePointShadow(shadowMap_cube4, lightDirection);
            }
            if (i == 5 && i<maxShadowRenderCount) {
                pointVisibility = samplePointShadow(shadowMap_cube5, lightDirection);
            }
            if (i == 6 && i<maxShadowRenderCount) {
                pointVisibility = samplePointShadow(shadowMap_cube6, lightDirection);
            }
            if (i == 7 && i<maxShadowRenderCount) {
                pointVisibility = samplePointShadow(shadowMap_cube7, lightDirection);
            }
            if (i == 8 && i<maxShadowRenderCount) {
                pointVisibility = samplePointShadow(shadowMap_cube8, lightDirection);
            }
            if (i == 9 && i<maxShadowRenderCount) {
                pointVisibility = samplePointShadow(shadowMap_cube9, lightDirection);
            }
            vec3 lightVector = normalize(lightSources[i]-vec3(fragPosition));
            diffuseColor += clamp(dot(normalize(vNormal), lightVector)
            *diffuseFactor*intensity*lightColors[i]*pointVisibility, 0.0, 1.0);
            vec3 cameraVector = normalize(camera - vec3(fragPosition));
            specularColor += clamp(pow((dot((cameraVector+lightVector),normalize(vNormal))/
            (length(cameraVector+lightVector)*length(normalize(vNormal)))),shininess)*pointVisibility, 0.0, 1.0)
            *specularFactor*intensity*lightColors[i];
        }
    }


    vec3 finalColor = specularColor + diffuseColor + ambientColor;
    float distanceCameraCenter = distance(cameraCenter, vec3(fragPosition));
    float fogFactor = clamp((1.0 - ((farPlane - 35.0) -distanceCameraCenter)/30.0), 0.0, 1.0);
    fogFactor *= clamp((1.0-((fragPosition.y-40.0)/30.0)), 0.0, 1.0);

    vec4 textureColor = vec4(0.0, 0.0, 0.0, 1.0);
    if (movingTexture == true) {
        textureColor = texture(uTexture, vec2(vTexCoord.x + movingTextureOffset.x, vTexCoord.y + movingTextureOffset.y)).rgba;
    }
    else {
        textureColor = texture(uTexture, vTexCoord).rgba;
    }
    oColor = vec4(finalColor, 1.0f)*textureColor;
    oColor = mix(oColor, fogColor(sunAngle), fogFactor);
}
