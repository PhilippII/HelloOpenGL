#shader vertex
#version 330 core

// "If the vertex shader has fewer components than the attribute provides, then the extras are ignored.
//  If the vertex shader has more components than the array provides, the extras are given values
//  from the vector (0, 0, 0, 1) for the missing XYZW components."
//   source: https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_format
//   -> I make position a vec4 rather than vec3
in vec4 position_oc;
in vec3 normal_oc;
uniform mat4 u_ndc_from_oc;
uniform mat4 u_cc_from_oc;

out vec3 normal_cc; // not yet normalized due to interpolation !
out vec3 posToLight_cc;

void main()
{
    gl_Position = u_ndc_from_oc * position_oc;
    normal_cc = u_cc_from_oc * vec4(normal_oc, 0.f); // assuming the model and view transforms preserve angles (e.g. no shear)
    vec4 pos_cc = u_cc_from_oc * position_oc;
    posToLight_cc = vec3(0.f) - pos_cc.xyz;
}

#shader fragment
#version 330 core
// light properties:
uniform vec3 u_i_s;
uniform vec3 u_i_d;
uniform vec3 u_i_a;

// material properties:
uniform vec3 u_k_s;
uniform vec3 u_k_d;
uniform vec3 u_k_a;
uniform vec3 u_shininess;

// vectors/normal:
uniform vec3 u_L_cc;
in vec3 normal_cc; // -> N_cc
// R_cc needs to be computed
in vec3 posToLight_cc; // -> V_cc

layout(location = 0) out vec4 out_color;

void main()
{
    vec3 N_cc = normalize(normal_cc);
    float L_dot_N = dot(u_L_cc, N_cc);
    vec3 R_cc = 2.f * L_dot_N * N_cc - u_L_cc;
    vec3 V_cc = normalize(posToLight_cc);
    vec3 color = u_k_a * u_i_a
                 + L_dot_N * u_k_d * u_i_d
                 + pow(dot(R_cc, V_cc), u_shininess) * u_k_s * u_i_s;
    // TODO: precompute the products u_k_? * u_i_? for ? = a, d, s
    out_color = vec4(color, 1.f);
}

