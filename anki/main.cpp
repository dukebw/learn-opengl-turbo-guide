#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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
        };
        uint32_t indices[] = {
        };
        uint32_t VAO = init_VAO(vertices,
                                sizeof(vertices),
                                indices,
                                sizeof(indices));

        init_vert_attr(0, 0, 0, 0);

        uint32_t texture;
        glGenTextures(1, &texture);

        // set the texture wrapping/filtering options (on the currently bound
        // texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int32_t width;
        int32_t height;
        int32_t nrChannels;
        uint8_t *data = stbi_load("cvpr2018_log_512x512.jpg",
                                  &width,
                                  &height,
                                  &nrChannels,
                                  0);
        assert(data != NULL);

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGB,
                     width,
                     height,
                     0,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        glUseProgram(shader_prog);
        while (!glfwWindowShouldClose(window)) {
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                glfwSwapBuffers(window);
                glfwPollEvents();
        }

        glfwTerminate();

        return EXIT_SUCCESS;
}
