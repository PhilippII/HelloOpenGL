#shader vertex
#version 330 core
in vec4 position;
in vec4 vs_in_color;
out vec4 fs_in_color;

uniform vec4 u_Color;

void main()
{
   fs_in_color = vec4(vs_in_color.rgb * vs_in_color.a + (1.f - vs_in_color.a) * u_Color.rgb, 1.f);
   gl_Position = position;
}

#shader fragment
#version 330 core
in vec4 fs_in_color;
layout(location = 0) out vec4 color;


void main()
{
   color = fs_in_color;
}
