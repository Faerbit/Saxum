#version 150

uniform vec3 camera;

in vec3 fColor;

in GS_OUT {
    smooth vec3 normal;
    smooth vec3 position;
}fs_in;

out vec4 oColor;


void main() {
    float dotProduct = dot(normalize(fs_in.normal), normalize(camera - fs_in.position));
    if (dotProduct < 0.1 && dotProduct > 0.0){
        oColor = vec4(1.0, 0.0, 0.0, 0.5);
    }
    else {
        oColor = vec4(fColor, 0.5);
    }
}
