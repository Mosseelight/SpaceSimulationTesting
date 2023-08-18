#version 460 core

out vec4 out_color;
in vec3 normal;
in vec3 pos;

uniform vec3 color;

void main()
{
    vec3 lightPos = vec3(0,4,4);
    vec3 nor = normalize(normal);
    float lightAng = dot(normalize(lightPos - pos), nor);
    float scatter = 0.85f;
    vec3 light = lightAng * scatter * vec3(1,1,1);
    out_color = vec4(color * light, 1.0f);
}