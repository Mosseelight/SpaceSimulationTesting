#version 330 core

out vec4 out_color;
in vec3 normal;
in vec3 pos;

uniform vec3 color;

void main()
{
    vec3 ambient = 0.1f * vec3(0.0f, 0.54f, 0.54f);
    vec3 norm = normalize(normal);
    vec3 lightdir = normalize(vec3(0,4,4) - pos);
    float diff = abs(max(dot(norm, lightdir), 0.0));
    vec3 diffuse = diff * vec3(1.0f, 1.0f, 1.0f);
    out_color = vec4((diffuse + ambient) * color, 1.0f);
}