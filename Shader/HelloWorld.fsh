#version 150

in vec3 vNormal;
in vec2 vTexCoord;

out vec4 oColor;

uniform sampler2D uTexture;

void main()
{
    vec3 color = texture(uTexture, vTexCoord).rgb;
    color *= dot(normalize(vNormal),vec3(0,0,1));

    oColor = vec4( color, 1.0 );
}
