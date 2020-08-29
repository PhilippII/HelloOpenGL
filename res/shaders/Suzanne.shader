#shader vertex
#version 330 core
in vec4 position_oc;
in vec3 normal_oc;
out vec3 normal_ndc_z_backward;

void main()
{
    gl_Position = position_oc;
    normal_ndc_z_backward = normal_oc * vec3(1.f, 1.f, -1.f);
}

#shader fragment
#version 330 core
in vec3 normal_ndc_z_backward;
layout(location = 0) out vec4 color;


void main()
{
    vec3 N = normalize(normal_ndc_z_backward);
    color = vec4(.5f * N + vec3(.5), 1.f);
}
