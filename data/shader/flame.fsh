#version 150

uniform vec3 camera;

in vec3 fColor;

in GS_OUT {
    vec3 normal;
}fs_in;

out vec4 oColor;


void main() {
    float dotProduct = dot(fs_in.normal, camera);
    vec3 color = fColor * dotProduct;
    oColor = vec4(color, 0.5);
}
