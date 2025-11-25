#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>

#include <shader.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, 1);
    }
}

int main()
{
    printf("MATH-182: A custom game engine in C for learning and fun\nBy Willard Nilges\n");
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "MATH-182", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        return -1;
    }

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    printf("Maximum number of vertex attributes supported: %d\n", nrAttributes);

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Shader* shaderProgram = newShader(
        "shaders/shader.vert",
        "shaders/shader.frag"
    );

    // Create a buffer and put some data in it
    //float vertices[] = {
    //    // Positions         // Colors
    //      0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    //     -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
    //      0.0f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f
    //};

    float vertices[] = {
        // Positions
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.0f, 0.5f, 0.0f, 
    };

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Wireframe mode-nt

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        float timeValue = glfwGetTime();
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;

        //FIXME: My struct isn't working :(
        //shaderUse(shaderProgram);
        //shaderSetFloat(shaderProgram, "greenFactor", greenValue);

        // Render stuff!!!!
        glClearColor(0.2f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            vertices,
            GL_STATIC_DRAW
        );

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        shaderUse(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap buffers!
        glfwSwapBuffers(window);
        // Read inputs!
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
