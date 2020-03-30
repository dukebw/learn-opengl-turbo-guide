#version 330 core
in vec3 vert_colour;
in vec2 vert_texcoord;

out vec4 frag_colour;

uniform sampler2D host_texture;

void main()
{
        frag_colour = texture(host_texture, vert_texcoord) * vec4(vert_colour, 1.0);
}
