#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform float horiz_offs;

out vec3 ourColor;

void main()
{
    gl_Position = vec4(aPos.x + horiz_offs, -aPos.y, aPos.z, 1.0);
    ourColor = vec3(aPos.x + 0.5, aPos.y + 0.5, 0.0);
}
