#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include "cglm/mat4.h"
#include "cglm/util.h"
#include "light.h"
#include "model.h"
#include "shader.h"
#include "camera.h"
#include "stb_image.h"

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

    // Flip images vertically when we load them
    stbi_set_flip_vertically_on_load(true);

    // Enable Depth Buffer
    glEnable(GL_DEPTH_TEST);

    // Mouse look
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Scroll to zoom
    glfwSetScrollCallback(window, scroll_callback);

    // Initialize the camera
    camera = newCameraWithDefaults();

    // Set up a shader for our backpack
    Shader* mainShader = newShader(
        "shaders/main/shader.vert",
        "shaders/main/shader.frag"
    );
    if (mainShader == NULL) {
        printf("I'm outta here!\n");
        glfwTerminate();
        return -1;
    }

    // XXX: Need to declare it like this so that dirname can edit it later :/
    char backpackModelPath[] = "models/backpack/backpack.obj";
    Model* backpack = newModel(backpackModelPath);

    char floorModelPath[] = "models/plane/plane.obj";
    Model* floor = newModel(floorModelPath);

    

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Clear the screen with a color
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        cameraUpdateMatricies(camera, windowWidth, windowHeight);

        // Set up the directional light
        DirLight dirLight;
        dirLight_setDirection(&dirLight, -0.2f, -1.0f, -0.3f);
        vec3 viewspaceLightDir;
        glm_mat4_mulv3(camera->view, dirLight.direction.raw, 1.0, viewspaceLightDir);

        // Lighting color information
        dirLight_setAmbient(&dirLight, 0.1f, 0.1f, 0.1f);
        dirLight_setDiffuse(&dirLight, 0.5f, 0.5f, 0.5f);
        dirLight_setSpecular(&dirLight, 1.0f, 1.0f, 1.0f);

        mat4 backpackModel;
        glm_mat4_identity(backpackModel);
        vec3 backpackPosition = { 0.0f, 0.0f, 0.0f };
        glm_translate(backpackModel, backpackPosition);

        shaderUse(mainShader);
        shaderSetMat4v(mainShader, "view", camera->view);
        shaderSetMat4v(mainShader, "projection", camera->projection);
        shaderSetVec3(mainShader, "dirLight.direction", viewspaceLightDir);
        shaderSetVec3(mainShader, "dirLight.ambient", dirLight.ambient.raw);
        shaderSetVec3(mainShader, "dirLight.diffuse", dirLight.diffuse.raw);
        shaderSetVec3(mainShader, "dirLight.specular", dirLight.specular.raw);
        shaderSetMat4v(mainShader, "model", backpackModel);

        // Draw models
        model_draw(floor, mainShader);
        model_draw(backpack, mainShader);

        glfwSwapBuffers(window); // Swap buffers!
        glfwPollEvents(); // Read inputs!
    }

    glfwTerminate();
    return 0;
}
