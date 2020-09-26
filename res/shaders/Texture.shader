#shader vertex
#version 330 core
in vec4 position_oc;
in vec2 texCoord;
out vec2 texCoord_v;

void main()
{
    gl_Position = position_oc;
    texCoord_v = texCoord;
}

#shader fragment
#version 330 core
in vec2 texCoord_v;
layout(location = 0) out vec4 color;

uniform sampler2D tex;

void main()
{
    // "The components not stored by the image format are filled in automatically.
    //  Zeros are used if R, G, or B is missing, while a missing Alpha always resolves to 1."
    // from https://www.khronos.org/opengl/wiki/Image_Format#Color_formats
    color = texture(tex, texCoord_v);
}
