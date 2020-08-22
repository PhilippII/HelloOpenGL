#shader vertex
#version 330 core
layout(location=0) in vec4 position_ndc;
layout(location=2) in vec3 normal_ndc;
out vec3 normal_ndc_fs_in;

void main()
{
    gl_Position = position_ndc;
    normal_ndc_fs_in = normal_ndc;
}

#shader fragment
#version 330 core
in vec3 normal_ndc_fs_in;
layout(location = 0) out vec4 color;


void main()
{
    color = vec4(.5f * normal_ndc_fs_in + vec3(.5), 1.f);
}
