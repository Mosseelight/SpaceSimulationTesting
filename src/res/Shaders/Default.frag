#version 330 core

out vec4 out_color;
in vec3 normal;
in vec3 pos;
in vec2 uv;

uniform vec3 color;
uniform vec3 viewPos;
uniform sampler2D tex;

void main()
{
    vec4 finalColor;
    vec4 colorNew = vec4(color,1.0);
    vec4 ambient = vec4(0.0,0.34,0.34,1.0);
    vec4 texelColor;
    if (all(equal(uv, vec2(0.0))))
    {
        texelColor = vec4(1.0);
    }
    else
    {
        texelColor = texture(tex, uv);
    }
    vec3 lightDot = vec3(0.0);
    vec3 nor = normalize(normal);
    vec3 viewD = normalize(viewPos - pos);
    vec3 specular = vec3(0.0);

    vec3 light = vec3(0.0);
    light = normalize(vec3(0,10,0) - pos);
    float NdotL = max(dot(nor, light), 0.0);
    lightDot += vec3(1.0,1.0,1.0)*NdotL;

    float specCo = 0.0;
    if (NdotL > 0.0) specCo = pow(max(0.0, dot(viewD, reflect(-(light), nor))), 128.0); 
    specular += specCo;

    finalColor = (texelColor *((colorNew + vec4(specular, 1.0))*vec4(lightDot, 1.0)));
    finalColor += texelColor*(ambient/10.0)*colorNew;

    out_color = pow(finalColor, vec4(1.0/2.2));
}