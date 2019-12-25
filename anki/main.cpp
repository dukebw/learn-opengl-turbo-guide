#include "learngl.hpp"
#include <cstdlib>

int main(void)
{
        GLFWwindow* window = init_gl(800, 600);
        if (window == NULL)
                return -1;

        uint32_t vertex = create_shader("./shader.vert", GL_VERTEX_SHADER);
        uint32_t fragment = create_shader("./shader.frag", GL_FRAGMENT_SHADER);
        uint32_t shader_prog = create_shader_program(vertex, fragment);
        float vertices[] = {
                -0.5f, -0.5f, 0.0f,
                0.5f, -0.5f, 0.0f,
                0.5f,  0.5f, 0.0f,
        };
        uint32_t VAO = init_VAO(vertices, sizeof(vertices));

        return EXIT_SUCCESS;
}
