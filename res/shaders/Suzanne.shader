#shader vertex
#version 330 core
layout(location=0) in vec4 position_oc;
layout(location=2) in vec3 normal_oc;
out vec3 normal_ndc;

void main()
{
    gl_Position = position_oc;
    normal_ndc = normal_oc;
}

#shader fragment
#version 330 core
in vec3 normal_ndc;
layout(location = 0) out vec4 color;


void main()
{
    color = vec4(.5f * normal_ndc + vec3(.5), 1.f);
}
