#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;

uniform mat4 model;
uniform mat4 norModel;
uniform mat4 view;
uniform mat4 proj;

out vec3 normal;
out vec3 pos;
out vec2 uv;

void main()
{
    pos = vec3(model * vec4(aPosition, 1.0));
    normal = vec3(norModel * vec4(aNormal, 1.0));
    uv = aUv;
    gl_Position = proj * view * model * vec4(aPosition, 1.0);
}