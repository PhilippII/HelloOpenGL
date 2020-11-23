// blend vertex colors with uniform color
#shader vertex
#version 330 core
in vec4 position_oc;
in vec4 color;
out vec4 v_color;

uniform vec4 u_Color;

uniform mat4 u_ndc_from_oc;

void main()
{
   v_color = vec4(color.rgb * color.a + (1.f - color.a) * u_Color.rgb, 1.f);
   gl_Position = u_ndc_from_oc * position_oc;
}

#shader fragment
#version 330 core
in vec4 v_color;
layout(location = 0) out vec4 out_color;


void main()
{
   out_color = v_color;
}
