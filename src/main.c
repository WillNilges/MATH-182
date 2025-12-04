#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "cglm/affine.h"
#include "cglm/cglm.h"
#include "cglm/io.h"
#include "cglm/types.h"
#include "stb_image.h"
#include "shader.h"
#include "camera.h"

// Epic face opacity
float visibility = 0.2f;

// Wall-clock time for correct movement
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Camera Stuff
Camera* camera;

// Mouse look stuff
float lastX = 400, lastY = 300;
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    cameraProcessMouse(camera, xoffset, yoffset, true);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cameraProcessScroll(camera, yoffset);
}

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

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        visibility += 0.05f;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        visibility -= 0.05f;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraProcessKeyboard(camera, FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraProcessKeyboard(camera, BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraProcessKeyboard(camera, LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraProcessKeyboard(camera, RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        cameraProcessKeyboard(camera, UP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
    {
        cameraProcessKeyboard(camera, DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera->sprinting = true;
    } else {
        camera->sprinting = false;
    }
}

int main()
{
    printf("MATH-182: A custom game engine in C for learning and fun\nBy Willard Nilges\n");

    // Set up glfw window stuff
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int windowWidth = 800;
    int windowHeight = 600;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "MATH-182", NULL, NULL);
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

    // Mouse look
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Scroll to zoom
    glfwSetScrollCallback(window, scroll_callback);

    // Initialize the camera
    camera = newCameraWithDefaults();

    Shader* shaderProgram = newShader(
        "shaders/gouraud/shader.vert",
        "shaders/gouraud/shader.frag"
    );

    if (shaderProgram == NULL) {
        printf("I'm outta here!\n");
        glfwTerminate();
        return -1;
    }

    Shader* lightSourceShaderProgram = newShader(
        "shaders/shader.vert",
        "shaders/lightSource.frag"
    );

    if (lightSourceShaderProgram == NULL) {
        printf("I'm outta here!\n");
        glfwTerminate();
        return -1;
    }

    // Create a buffer and put some data in it. This is our cube "model"
    // with some texture data attached.
    float vertices[] = {
        // Positions          // Normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };



    // Create Vertex Array Objects and Vertex Buffer Objects
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Wireframe mode-nt

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // We need the normal data
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Need another VAO for the light
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // Bind the original buffer, because it already contains the data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Enable Depth Buffer
    glEnable(GL_DEPTH_TEST);

    // We need to define a point that the light emmenates from. This should
    // be the same as the position of the cube that represents the light.
    vec3 lightPos = { 2.0f, 3.0f, -3.0f };
    vec3 cubeLightPositions[] = {
        { lightPos[0], lightPos[1], lightPos[2] }
    };

    // And a cube to be hit by the light
    vec3 cubePositions[] = {
        { 2.0f,  1.0f, -3.0f}, 
    };

    size_t nCubePositions = sizeof(cubePositions)/sizeof(cubePositions[0]);

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        lightPos[2] = sin(currentFrame) * 5.0f;
        lightPos[0] = cos(currentFrame) * 5.0f;

        // Render stuff!!!!
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // COOL ASS FUCKING CUBE
        vec3 lightColor = { 1.0f, 1.0f, 1.0f };
        vec3 objectColor = { 0.1f, 1.0f, 1.0f };

        // Shader for the cuuuuubes!
        shaderUse(shaderProgram);
        shaderSetVec3(shaderProgram, "objectColor", objectColor[0], objectColor[1], objectColor[2]);
        shaderSetVec3(shaderProgram, "lightColor", lightColor[0], lightColor[1], lightColor[2]);
        shaderSetVec3(shaderProgram, "lightPos", lightPos[0], lightPos[1], lightPos[2]);
        shaderSetVec3(shaderProgram, "viewPos", camera->pos[0], camera->pos[1], camera->pos[2]);

        // --- 3D!!! ---
        //mat4 model;
        //glm_mat4_identity(model);

        mat4 view;
        cameraGetViewMatrix(camera, view);
        shaderSetMat4v(shaderProgram, "view", view);

        mat4 projection;
        glm_mat4_identity(projection);
        glm_perspective(glm_rad(camera->fov), (float)windowWidth/(float)windowHeight, 0.1f, 100.0f, projection);

        // Send the matricies to the shaders
        shaderSetMat4v(shaderProgram, "view", view);
        shaderSetMat4v(shaderProgram, "projection", projection);
        // --- /3D ---

        // DRAW THE FUCKING CUBE!
        glBindVertexArray(VAO);

        for (unsigned int i = 0; i < nCubePositions; i++)
        {
            mat4 cubeModel;
            glm_mat4_identity(cubeModel);
            glm_translate(cubeModel, cubePositions[i]);
            /*
            float angle = 20.0f * i;
            vec3 cubeAxis = { 1.0f, 0.3f, 0.5f };
            if (i % 3 == 0)
            {
                angle += glfwGetTime();
            }
            glm_rotate(cubeModel, angle, cubeAxis);
            */
            shaderSetMat4v(shaderProgram, "model", cubeModel);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // --- Draw the light!!! ---
        shaderUse(lightSourceShaderProgram);

        //mat4 view;
        cameraGetViewMatrix(camera, view);

        //mat4 projection;
        glm_mat4_identity(projection);
        glm_perspective(glm_rad(camera->fov), (float)windowWidth/(float)windowHeight, 0.1f, 100.0f, projection);

        // Send the matricies to the shaders
        shaderSetMat4v(shaderProgram, "view", view);
        shaderSetMat4v(shaderProgram, "projection", projection);

        glBindVertexArray(lightVAO);

        mat4 lightCubeModel;
        glm_mat4_identity(lightCubeModel);
        glm_translate(lightCubeModel, lightPos);
        vec3 lightCubeModelScale = { 0.2f, 0.2f, 0.2f };
        glm_scale(lightCubeModel, lightCubeModelScale);
        // TODO: Can't I replace this with something in my shader?
        shaderSetMat4v(lightSourceShaderProgram, "model", lightCubeModel);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Unbind our vertex array
        glBindVertexArray(0);

        // Swap buffers!
        glfwSwapBuffers(window);
        // Read inputs!
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
