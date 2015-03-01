#version 150

uniform mat4 viewProjectionMatrix;
uniform float time;

layout(points) in;
layout(triangle_strip, max_vertices = 256) out;

const float PI = 3.1415926;
const float transition_point = 1.178097;
const float sin_p1  = 0.4;
const float sin_p2  = 2;
const float ex_p1   = 1.093;
const float ex_p2   = 1.9996;
const float begin   = 0;
const float end     = 2.3;

float radiusFunction(float x) {
    if (x < transition_point) {
        return sin_p1 * sin(sin_p2 * x);
    }
    else {
        return exp(ex_p1 - ex_p2 * x);
    }
}

void main() {
    float resolution = 8.0;
    float step = abs(end-begin)/resolution;
    for (float i = begin; i<end; i+=step) {
        float downRadius    = radiusFunction(i);
        float upRadius = 0.0f;
        if (i != end-step) {
            upRadius = radiusFunction(i+step);
        }
        for (int j = 0; j<resolution; j++) {
            float leftAngle     = PI * 2.0 / resolution * j;
            float rightAngle    = PI * 2.0 / resolution * (j+1);

            vec4 offset = vec4(cos(leftAngle) * downRadius, i, -sin(leftAngle) * downRadius, 0.0);
            gl_Position = gl_in[0].gl_Position + viewProjectionMatrix * offset;
            EmitVertex();

            offset = vec4(cos(leftAngle) * upRadius, i + step, -sin(leftAngle) * upRadius, 0.0);
            gl_Position = gl_in[0].gl_Position + viewProjectionMatrix * offset;
            EmitVertex();

            offset = vec4(cos(rightAngle) * downRadius, i, -sin(rightAngle) * downRadius, 0.0);
            gl_Position = gl_in[0].gl_Position + viewProjectionMatrix * offset;
            EmitVertex();

            offset = vec4(cos(rightAngle) * upRadius, i + step, -sin(rightAngle) * upRadius, 0.0);
            gl_Position = gl_in[0].gl_Position + viewProjectionMatrix * offset;
            EmitVertex();

            EndPrimitive();
        }
    }

}
