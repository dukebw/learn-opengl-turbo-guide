#include "learngl.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#define UP 1
#define NEUTRAL 0
#define DOWN -1
#define SCR_WIDTH 800
#define SCR_HEIGHT 600

static float
process_input(GLFWwindow *window,
              uint32_t shader_prog,
              float alpha,
              int32_t& prev_state)
{
        constexpr float amt = 0.1f;
        int32_t uni_loc;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
        if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) &&
            (prev_state != UP)) {
                alpha = fmin(alpha + amt, 1.0f);
                prev_state = UP;

                uni_loc = glGetUniformLocation(shader_prog, "alpha");
                glUniform1f(uni_loc, alpha);
        }
        if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && (prev_state != DOWN)) {
                alpha = fmax(alpha - amt, 0.0f);
                prev_state = DOWN;

                uni_loc = glGetUniformLocation(shader_prog, "alpha");
                glUniform1f(uni_loc, alpha);
        }
        if ((glfwGetKey(window, GLFW_KEY_DOWN) != GLFW_PRESS) &&
            (glfwGetKey(window, GLFW_KEY_UP) != GLFW_PRESS) &&
            (prev_state != NEUTRAL))
                prev_state = NEUTRAL;

        return alpha;
}

static uint32_t
gen_texture(const char* path, int32_t format)
{
        uint32_t texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
        if (data) {
                glTexImage2D(GL_TEXTURE_2D,
                             0,
                             format,
                             width,
                             height,
                             0,
                             format,
                             GL_UNSIGNED_BYTE,
                             data);
                glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
                std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);

        return texture;
}

static void
set_mat4(uint32_t shader_prog, glm::mat4& transform, const char* uni_name)
{
        uint32_t trans_loc = glGetUniformLocation(shader_prog, uni_name);
        glUniformMatrix4fv(trans_loc,
                           1,
                           GL_FALSE,
                           glm::value_ptr(transform));
}

int main(void)
{
        GLFWwindow* window = init_gl(SCR_WIDTH, SCR_HEIGHT);
        if (window == NULL)
                return -1;

        uint32_t vertex = create_shader("./shader.vs", GL_VERTEX_SHADER);
        uint32_t fragment = create_shader("./shader.fs", GL_FRAGMENT_SHADER);
        uint32_t shader_prog = create_shader_program(vertex, fragment);

        float vertices[] = {
                -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
                0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

                -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        };
        glm::vec3 cube_positions[] = {
                glm::vec3( 0.0f,  0.0f,  0.0f),
                glm::vec3( 2.0f,  5.0f, -15.0f),
                glm::vec3(-1.5f, -2.2f, -2.5f),
                glm::vec3(-3.8f, -2.0f, -12.3f),
                glm::vec3( 2.4f, -0.4f, -3.5f),
                glm::vec3(-1.7f,  3.0f, -7.5f),
                glm::vec3( 1.3f, -2.0f, -2.5f),
                glm::vec3( 1.5f,  2.0f, -2.5f),
                glm::vec3( 1.5f,  0.2f, -1.5f),
                glm::vec3(-1.3f,  1.0f, -1.5f)
        };
        uint32_t VAO = init_VAO(vertices, sizeof(vertices));

        init_vert_attr(0, 3, 5, 0);
        init_vert_attr(1, 2, 5, 3);

        stbi_set_flip_vertically_on_load(true);
        uint32_t texture1 = gen_texture("/home/bduke/work/LearnOpenGL/resources/textures/container.jpg",
                                        GL_RGB);
        uint32_t texture2 = gen_texture("/home/bduke/work/LearnOpenGL/resources/textures/awesomeface.png",
                                        GL_RGBA);

        glUseProgram(shader_prog);
        glUniform1i(glGetUniformLocation(shader_prog, "texture1"), 0);
        glUniform1i(glGetUniformLocation(shader_prog, "texture2"), 1);

        float alpha = 0.5f;
        int32_t uni_loc = glGetUniformLocation(shader_prog, "alpha");
        glUniform1f(uni_loc, alpha);
        int32_t prev_state = NEUTRAL;

        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT,
                                                0.1f,
                                                100.0f);
        glUseProgram(shader_prog);
        set_mat4(shader_prog, projection, "projection");

        glEnable(GL_DEPTH_TEST);

        while (!glfwWindowShouldClose(window)) {
                alpha = process_input(window, shader_prog, alpha, prev_state);

                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture1);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, texture2);

                glm::mat4 view = glm::mat4(1.0f);
                view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

                glUseProgram(shader_prog);
                set_mat4(shader_prog, view, "view");

                glBindVertexArray(VAO);
                for (uint32_t i = 0;
                     i < sizeof(cube_positions)/sizeof(cube_positions[0]);
                     i++) {
                        // calculate the model matrix for each object and pass it to shader before drawing
                        glm::mat4 model = glm::mat4(1.0f);
                        model = glm::translate(model, cube_positions[i]);
                        float angle = 20.0f * i * glfwGetTime();
                        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                        set_mat4(shader_prog, model, "model");

                        glDrawArrays(GL_TRIANGLES, 0, 36);
                }

                glfwSwapBuffers(window);
                glfwPollEvents();
        }

        glfwTerminate();

        return EXIT_SUCCESS;
}
