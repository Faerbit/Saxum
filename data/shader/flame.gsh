#version 150

uniform mat4 viewProjectionMatrix;
uniform float time;

layout(points) in;
layout(triangle_strip, max_vertices = 64) out;

const float PI = 3.1415926;

void main() {
    float resolution = 5.0;
    for (int i = 0; i<resolution; i++) {
        float leftAngle     = PI * 2.0 / resolution * i;
        float rightAngle     = PI * 2.0 / resolution * (i+1);
        float downRadius    = 1.0;
        float upRadius      = 2.0;

        vec4 offset = vec4(cos(leftAngle) * downRadius, 0.0, -sin(leftAngle) * downRadius, 0.0);
        gl_Position = gl_in[0].gl_Position + viewProjectionMatrix * offset;
        EmitVertex();

        offset = vec4(cos(leftAngle) * upRadius, 1.0, -sin(leftAngle) * upRadius, 0.0);
        gl_Position = gl_in[0].gl_Position + viewProjectionMatrix * offset;
        EmitVertex();

        offset = vec4(cos(rightAngle) * downRadius, 0.0, -sin(rightAngle) * downRadius, 0.0);
        gl_Position = gl_in[0].gl_Position + viewProjectionMatrix * offset;
        EmitVertex();

        offset = vec4(cos(rightAngle) * upRadius, 1.0, -sin(rightAngle) * upRadius, 0.0);
        gl_Position = gl_in[0].gl_Position + viewProjectionMatrix * offset;
        EmitVertex();

        EndPrimitive();
    }

}
