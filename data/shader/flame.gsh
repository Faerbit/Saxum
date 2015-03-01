#version 150

uniform mat4 viewProjectionMatrix;
uniform float time;
uniform bool bottom;
uniform bool left;

layout(points) in;
layout(triangle_strip, max_vertices = 78) out;

out GS_OUT {
    smooth vec3 normal;
    smooth vec3 position;
}gs_out;

in vec3 Color[];
out vec3 fColor;

const float PI = 3.1415926;
const float transition_point = 1.178097;
const float sin_p1  = 0.4;
const float sin_p2  = 2;
const float ex_p1   = 1.093;
const float ex_p2   = 1.9996;
const float begin   = 0;
const float end     = 2.5;

float radiusFunction(float x) {
    if (x < transition_point) {
        return sin_p1 * sin(sin_p2 * x);
    }
    else {
        return exp(ex_p1 - ex_p2 * x);
    }
}

void main() {
    fColor = Color[0];

    float resolution = 6.0;
    float step = abs(end-begin)/resolution/2.0;
    float i = 0.0;
    float render_end = 0.0;
    if (bottom) {
        i = begin;
        render_end = (end-begin)/2.0+step;
    }
    else {
        i = (end-begin)/2.0;
        render_end = end;
    }
    for (i; i<render_end; i+=step) {
        float downRadius    = radiusFunction(i);
        float upRadius = radiusFunction(i+step);
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
            vec4 position = gl_in[0].gl_Position + viewProjectionMatrix * offset;
            gl_Position = position;
            gs_out.normal = vec3(position - gl_in[0].gl_Position);
            gs_out.position = vec3(position);
            EmitVertex();

            offset = vec4(cos(rightAngle) * upRadius, i + step, -sin(rightAngle) * upRadius, 0.0);
            position = gl_in[0].gl_Position + viewProjectionMatrix * offset;
            gl_Position = position;
            gs_out.normal = vec3(position - gl_in[0].gl_Position);
            gs_out.position = vec3(position);
            EmitVertex();

            offset = vec4(cos(leftAngle) * downRadius, i, -sin(leftAngle) * downRadius, 0.0);
            position = gl_in[0].gl_Position + viewProjectionMatrix * offset;
            gl_Position = position;
            gs_out.normal = vec3(position - gl_in[0].gl_Position);
            gs_out.position = vec3(position);
            EmitVertex();

            offset = vec4(cos(leftAngle) * upRadius, i + step, -sin(leftAngle) * upRadius, 0.0);
            position = gl_in[0].gl_Position + viewProjectionMatrix * offset;
            gl_Position = position;
            gs_out.normal = vec3(position - gl_in[0].gl_Position);
            gs_out.position = vec3(position);
            EmitVertex();

            EndPrimitive();
        }
    }

}
