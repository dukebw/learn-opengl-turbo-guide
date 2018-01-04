#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <assert.h>
#include <stdlib.h>

#define UNUSED(x) x __attribute__ ((__unused__))

void framebuffer_size_callback(GLFWwindow *UNUSED(window), int32_t width, int32_t height)
{
        glViewport(0, 0, width, height);
}

int main(void)
{
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow *window = glfwCreateWindow(800, 600, "Learn OpenGL", NULL, NULL);
        assert(window != NULL);

        assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != 0);

        glViewport(0, 0, 800, 600);

        assert(glfwSetFramebufferSizeCallback(window, framebuffer_size_callback) != NULL);

        return EXIT_SUCCESS;
}
