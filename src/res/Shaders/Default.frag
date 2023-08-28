#version 460 core

out vec4 out_color;
in vec3 normal;
in vec3 pos;
in vec2 uv;

uniform vec3 color;
uniform sampler2D tex;

void main()
{
    float shine = 20.0;

    float ambientPower = 0.185;
    vec3 ambientColor = vec3(0.0,0.54,0.54);

    vec3 lightDiffuse = vec3(1.0);

    vec3 lightSpec = vec3(0.5);

    vec3 lightPos = vec3(0,4,4);
    vec3 nor = normalize(normal);
    vec3 lightDir = normalize(lightPos - pos);
    vec3 v = normalize(-pos);
    vec3 r = reflect(-lightDir, nor);
    float LDDN = max(dot(lightDir, nor), 0.0);
    vec3 ambient = ambientColor * ambientPower;
    vec3 diffuse = lightDiffuse * LDDN;
    vec3 spec = vec3(0.0);
    if(LDDN > 0.0)
    {
        spec = lightSpec * pow(max(dot(r,v),0.0),shine);
    }
    vec3 light = ambient + diffuse + spec;
    out_color = vec4(light * color, 1.0);
}