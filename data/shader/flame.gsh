#version 150

uniform mat4 modelViewProjectionMatrix;
uniform float time;
uniform bool bottom;
uniform int circle_index;
uniform vec2 skew;

layout(points) in;
layout(triangle_strip, max_vertices = 146) out;

in vec3 Color[];
out vec3 fColor;

const float PI = 3.1415926;
const float transition_point_1 = 0.486946;
const float cos_p1_1  = 0.35;
const float cos_p2_1  = 2.0;
const float ex_p1_1   = -0.193;
const float ex_p2_1   = 2.9426;
const float begin_1   = -0.8;
const float end_1     = 2;

const float transition_point_2 = 0.549778;
const float cos_p1_2  = 0.5;
const float cos_p2_2  = 2.0;
const float ex_p1_2   = 0.677;
const float ex_p2_2   = 3.9314;
const float begin_2   = -0.8;
const float end_2     = 1.3;

float flickerFunction() {
    float windPower = length(skew);
    if (windPower < 0.8) {
        windPower += 0.2;
    }
    return windPower * pow(sin((20.0/(windPower))*time + gl_in[0].gl_Position.x*gl_in[0].gl_Position.z), 2) + (1-windPower);
}

float radiusFunction(float x) {
    float value_1 = 0.0;
    float value_2 = 0.0;
    if (x < transition_point_1) {
        value_1 = cos_p1_1 * cos(cos_p2_1 * x);
    }
    else {
        value_1 = exp(ex_p1_1 - ex_p2_1 * x);
    }
    if (x < transition_point_2) {
        value_2 = cos_p1_2 * cos(cos_p2_2 * x);
    }
    else {
        value_2 = exp(ex_p1_2 - ex_p2_2 * x);
    }
    return mix(value_1, value_2, flickerFunction());
}

void main() {
    fColor = Color[0];

    float resolution = 6.0;
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
        int j = int(resolution/2.0) * circle_index;
        int circle_end = int(resolution/2.0) * (circle_index + 1);
        for (j; j<circle_end; j++) {
            float leftAngle     = PI * 2.0 / resolution / 4.0  * j;
            float rightAngle    = PI * 2.0 / resolution / 4.0 * (j+1);

            vec4 offset = vec4(cos(rightAngle) * downRadius, i, -sin(rightAngle) * downRadius, 0.0);
            gl_Position = gl_in[0].gl_Position + modelViewProjectionMatrix * offset;
            EmitVertex();

            offset = vec4(cos(rightAngle) * upRadius, i + step, -sin(rightAngle) * upRadius, 0.0);
            gl_Position = gl_in[0].gl_Position + modelViewProjectionMatrix * offset;
            EmitVertex();

            offset = vec4(cos(leftAngle) * downRadius, i, -sin(leftAngle) * downRadius, 0.0);
            gl_Position = gl_in[0].gl_Position + modelViewProjectionMatrix * offset;
            EmitVertex();

            offset = vec4(cos(leftAngle) * upRadius, i + step, -sin(leftAngle) * upRadius, 0.0);
            gl_Position = gl_in[0].gl_Position + modelViewProjectionMatrix * offset;
            EmitVertex();

            EndPrimitive();
        }
    }

}
