#version 150

in GS_OUT {
    float maxAngle;
    vec3 position;
}fs_in;

in vec4 flameCenter;

out vec4 oColor;

uniform bool withColor;
uniform vec3 camera;

vec3 colorFunction(float quotient) {
    if (quotient < 0.2){
        return vec3(1.0, 0.0, 0.0);
    }
    else {
        return vec3(0.0, 1.0, 0.0);
    }
}

void main() {
    vec2 cameraXZ = vec2(camera.x, camera.z);
    vec3 cameraVector = vec3(flameCenter) - camera;
    vec2 cameraVectorXZ = normalize(vec2(cameraVector.x, cameraVector.z));
    vec2 rightAngleVector;
    if (cameraVectorXZ.y == 0.0) {
       rightAngleVector = vec2(0.0, 1.0); 
    }
    else {
        rightAngleVector = normalize(vec2(1.0, -cameraVectorXZ.x/cameraVectorXZ.y));
    }
    float angle = acos(dot(cameraVectorXZ, normalize(vec2(fs_in.position.x, fs_in.position.z)-cameraXZ)));
    if (withColor) {
        oColor = vec4(colorFunction(angle/fs_in.maxAngle), 0.6);
    }
    else {
        oColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}
