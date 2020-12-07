#shader vertex
#version 330 core
in vec4 ndc_pos;

void main()
{
    gl_Position = ndc_pos;
}

#shader fragment
#version 330 core
// in vec4 gl_FragCoord;
uniform sampler2D tex;
const float[3 * 3] filter = float[](1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f,
                                    2.0f/16.0f, 4.0f/16.0f, 2.0f/16.0f,
                                    1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f);

layout(location = 0) out vec4 out_color;

void main()
{
    ivec2 maxCoord = textureSize(tex, 0) - ivec2(1);
    vec3 res = vec3(0.f);
    for (int offX = -1; offX <= 1; ++offX) {
        for (int offY = -1; offY <= 1; ++offY) {
            ivec2 unnormTexCoord = clamp(ivec2(gl_FragCoord.xy) + ivec2(offX, offY), ivec2(0), maxCoord);
            vec3 in_color = texelFetch(tex, unnormTexCoord, 0).rgb;
            int col = offX + 1;
            int row = 1 - offY;
            res += filter[3*row + col] * in_color;
       }
    }
    out_color = vec4(res, 1.f);
}


