#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_colour;
layout (location = 2) in vec2 a_texcoord;

out vec3 vert_colour;
out vec2 vert_texcoord;

void main()
{
        gl_Position = vec4(a_pos, 1.0);
        vert_texcoord = a_texcoord;
        vert_colour = a_colour;
}
