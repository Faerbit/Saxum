#version 150

uniform mat4 modelViewProjectionMatrix;
uniform mat4 viewProjectionMatrix;
uniform float time;
uniform bool bottom;
uniform bool left;
uniform vec3 camera;

layout(points) in;
layout(triangle_strip, max_vertices = 85) out;

out GS_OUT {
    float maxAngle;
    vec3 position;
}gs_out;

in vec3 Color[];
out vec4 flameCenter;

const float PI = 3.1415926;
const float transition_point_1 = 1.178097;
const float sin_p1_1  = 0.4;
const float sin_p2_1  = 2;
const float ex_p1_1   = 1.093;
const float ex_p2_1   = 1.9996;
const float begin_1   = 0;
const float end_1     = 2.5;

const float transition_point_2 = 1.570796;
const float sin_p1_2  = 0.2;
const float sin_p2_2  = 1.5;
const float ex_p1_2   = 0.4;
const float ex_p2_2   = 1.5;
const float begin_2   = 0;
const float end_2     = 3;

float flickerFunction() {
    //return pow(0.6*sin(20.0*time), 2) + 0.4;
    return 1.0;
}


float radiusFunction(float x) {
    float value_1 = 0.0;
    float value_2 = 0.0;
    if (x < transition_point_1) {
        value_1 = sin_p1_1 * sin(sin_p2_1 * x);
    }
    else {
        value_1 = exp(ex_p1_1 - ex_p2_1 * x);
    }
    if (x < transition_point_2) {
        value_2 = sin_p1_2 * sin(sin_p2_2 * x);
    }
    else {
        value_2 = exp(ex_p1_2 - ex_p2_2 * x);
    }
    return mix(value_1, value_2, flickerFunction());
}

void main() {
    flameCenter = gl_in[0].gl_Position;
    vec3 color = Color[0];
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

    float resolution = 8.0;
    float this_begin = mix(begin_1, begin_2, flickerFunction());
    float this_end = mix(end_1, end_2, flickerFunction());
    float step = abs(this_end-this_begin)/resolution/2.0;
    float i = 0.0;
    float render_end = 0.0;
    if (bottom) {
        i = this_begin;
        render_end = (this_end-this_begin)/2.0+step;
    }
    else {
        i = (this_end-this_begin)/2.0;
        render_end = this_end;
    }
    for (i; i<render_end; i+=step) {
        float downRadius    = radiusFunction(i);
        float upRadius = radiusFunction(i+step);
        float maxUpAngle = acos(dot(cameraVectorXZ, normalize((vec2(gl_in[0].gl_Position.x, gl_in[0].gl_Position.z) + rightAngleVector * upRadius) - cameraXZ)));
        float maxDownAngle = acos(dot(cameraVectorXZ, normalize((vec2(gl_in[0].gl_Position.x, gl_in[0].gl_Position.z) + rightAngleVector * downRadius) - cameraXZ)));
        float circle_end = 0.0;
        int j = 0;
        if (left) {
            j = 0;
            circle_end = resolution/2.0;
        }
        else {
            j = int(resolution/2.0);
            circle_end = resolution;
        }
        for (j; j<circle_end; j++) {
            float leftAngle     = PI * 2.0 / resolution   * j;
            float rightAngle    = PI * 2.0 / resolution * (j+1);

            vec4 offset = vec4(cos(rightAngle) * downRadius, i, -sin(rightAngle) * downRadius, 0.0);
            gs_out.position = vec3(gl_in[0].gl_Position + offset);
            gs_out.maxAngle = maxDownAngle;
            gl_Position = gl_in[0].gl_Position + modelViewProjectionMatrix * offset;
            EmitVertex();

            offset = vec4(cos(rightAngle) * upRadius, i + step, -sin(rightAngle) * upRadius, 0.0);
            gs_out.position = vec3(gl_in[0].gl_Position + offset);
            gs_out.maxAngle = maxUpAngle;
            gl_Position = gl_in[0].gl_Position + modelViewProjectionMatrix * offset;
            EmitVertex();

            offset = vec4(cos(leftAngle) * downRadius, i, -sin(leftAngle) * downRadius, 0.0);
            gs_out.position = vec3(gl_in[0].gl_Position + offset);
            gs_out.maxAngle = maxDownAngle;
            gl_Position = gl_in[0].gl_Position + modelViewProjectionMatrix * offset;
            EmitVertex();

            offset = vec4(cos(leftAngle) * upRadius, i + step, -sin(leftAngle) * upRadius, 0.0);
            gs_out.position = vec3(gl_in[0].gl_Position + offset);
            gs_out.maxAngle = maxUpAngle;
            gl_Position = gl_in[0].gl_Position + modelViewProjectionMatrix * offset;
            EmitVertex();

            EndPrimitive();
        }
    }

}
