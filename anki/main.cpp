#include "learngl.hpp"
#include <cassert>
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
                -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        };
        uint32_t indices[] = {
                0, 1, 2,
                2, 3, 0,
        };
        uint32_t VAO = init_VAO(vertices,
                                sizeof(vertices),
                                indices,
                                sizeof(indices));

        init_vert_attr(0, 3, 8, 0);
        init_vert_attr(1, 3, 8, 3);
        init_vert_attr(2, 2, 8, 6);

        uint32_t texture1;
        glGenTextures(1, &texture1);
        glBindTexture(GL_TEXTURE_2D, texture1);

        // set the texture wrapping/filtering options (on the currently bound
        // texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int32_t result = load_texture_image("container.jpg", GL_RGB);
        assert(result == 0);

        glUseProgram(shader_prog);
        while (!glfwWindowShouldClose(window)) {
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                glfwSwapBuffers(window);
                glfwPollEvents();
        }

        glfwTerminate();

        return EXIT_SUCCESS;
}
