#shader vertex
#version 330 core
layout(location=0) in vec4 position_ndc;
layout(location=2) in vec3 normal_ndc;
out vec3 normal_z_backward;

void main()
{
    gl_Position = position_ndc;
    normal_z_backward = normal_ndc * vec3(1.f, 1.f, -1.f);
}

#shader fragment
#version 330 core
in vec3 normal_z_backward;
layout(location = 0) out vec4 color;


void main()
{
    color = vec4(.5f * normal_z_backward + vec3(.5), 1.f);
}
