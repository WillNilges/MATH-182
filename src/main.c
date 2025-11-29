#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "cglm/cglm.h"
#include "cglm/mat4.h"
#include "cglm/util.h"
#include "cglm/vec3.h"
#include "stb_image.h"
#include "shader.h"

// Epic face opacity
float visibility = 0.2f;

// Camera Stuff
const float cameraSpeed = 0.1f; // adjust accordingly
float deltaTime = 0.0f;
float lastFrame = 0.0f;
vec3 cameraPos = { 0.0f, 0.0f, 3.0f };
vec3 cameraFront = { 0.0f, 0.0f, -1.0f };
vec3 cameraUp = { 0.0f, 1.0f, 0.0f };

// Mouse look stuff
float lastX = 400, lastY = 300;
bool firstMouse = true;

float pitch = 0.0f;
float yaw = -90.0f;
float roll = 0.0f;

float fov = 90.0f;

void lookAt(vec3 eye, vec3 center, vec3 up, mat4 dest)
{
    // Remember, multiply right to left

    // We create the two matricies we need to multiply.
    mat4 cameraPosition, rightUpDirection;
    glm_mat4_identity(cameraPosition);
    glm_mat4_identity(rightUpDirection);

    // Construct Camera Position Matrix. Use glm_mat4_make, which is column-wise
    /*
     *Input : mat[][] = {{0,  4,  8, 12}, 
                         {1,  5,  9, 13}, 
                         {2,  6, 10, 14},
                         {3,  7, 11, 15}}
     * */
    // I think this is column-major
    cameraPosition[3][0] = -eye[0];
    cameraPosition[3][1] = -eye[1];
    cameraPosition[3][2] = -eye[2];

    // f = dir
    // s = right

    // Create right vector
    vec3 dir;
    glm_vec3_sub(center, eye, dir);
    glm_vec3_normalize(dir);

    vec3 right;
    glm_vec3_crossn(dir, up, right);

    vec3 u;
    glm_vec3_cross(right, dir, u);

    rightUpDirection[0][0] = right[0];
    rightUpDirection[1][0] = right[1];
    rightUpDirection[2][0] = right[2];

    // Create up vector
    rightUpDirection[0][1] = u[0];
    rightUpDirection[1][1] = u[1];
    rightUpDirection[2][1] = u[2];

    rightUpDirection[0][2] = -dir[0];
    rightUpDirection[1][2] = -dir[1];
    rightUpDirection[2][2] = -dir[2];

    // Multiply the matricies, right to left
    glm_mat4_mul(rightUpDirection, cameraPosition, dest);
}

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

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
    {
        pitch = 89.0f;
    }

    if (pitch < -89.0f)
    {
        pitch = -89.0f;
    }

    vec3 direction = {
        cos(glm_rad(yaw)) * cos(glm_rad(pitch)),
        sin(glm_rad(pitch)),
        sin(glm_rad(yaw)) * cos(glm_rad(pitch))
    };

    glm_normalize(direction);
    glm_vec3_copy(direction, cameraFront);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f; 
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
        vec3 cameraSpeedXCameraFront;
        glm_vec3_scale(cameraFront, cameraSpeed, cameraSpeedXCameraFront);
        cameraSpeedXCameraFront[1] = 0.0f; // No vertical movement
        glm_vec3_add(cameraPos, cameraSpeedXCameraFront, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        vec3 cameraSpeedXCameraFront;
        glm_vec3_scale(cameraFront, cameraSpeed, cameraSpeedXCameraFront);
        cameraSpeedXCameraFront[1] = 0.0f; // No vertical movement
        glm_vec3_sub(cameraPos, cameraSpeedXCameraFront, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        vec3 cameraFrontCrossCameraUp;
        glm_cross(cameraFront, cameraUp, cameraFrontCrossCameraUp);
        glm_normalize(cameraFrontCrossCameraUp);

        glm_vec3_scale(cameraFrontCrossCameraUp, cameraSpeed, cameraFrontCrossCameraUp);
        cameraFrontCrossCameraUp[1] == 0.0f; // No vertical movement
        glm_vec3_sub(cameraPos, cameraFrontCrossCameraUp, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        vec3 cameraFrontCrossCameraUp;
        glm_cross(cameraFront, cameraUp, cameraFrontCrossCameraUp);
        glm_normalize(cameraFrontCrossCameraUp);

        glm_vec3_scale(cameraFrontCrossCameraUp, cameraSpeed, cameraFrontCrossCameraUp);
        cameraFrontCrossCameraUp[1] == 0.0f; // No vertical movement
        glm_vec3_add(cameraPos, cameraFrontCrossCameraUp, cameraPos);
    }
}

int main()
{
    printf("MATH-182: A custom game engine in C for learning and fun\nBy Willard Nilges\n");
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

    Shader* shaderProgram = newShader(
        "shaders/shader.vert",
        "shaders/shader.frag"
    );

    if (shaderProgram == NULL) {
        printf("I'm outta here!\n");
        glfwTerminate();
        return -1;
    }

    // Create a buffer and put some data in it
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

    unsigned int VAO, VBO;//, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);

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

    // No indices
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(
    //    GL_ELEMENT_ARRAY_BUFFER,
    //    sizeof(indices), 
    //    indices, 
    //    GL_STATIC_DRAW
    //);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // No uhhhhh color information?
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);  

    // --- Textures ---
    stbi_set_flip_vertically_on_load(1);
    // container.jpg
    // Generate texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load("textures/container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load texture\n");
        glfwTerminate();
        return -1;
    }
    stbi_image_free(data);

    // awesomeface.png
    // Generate texture
    unsigned int awesomeTexture;
    glGenTextures(1, &awesomeTexture);
    glBindTexture(GL_TEXTURE_2D, awesomeTexture);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int awesomeWidth, awesomeHeight, awesomeNrChannels;
    unsigned char *awesomeData = stbi_load("textures/awesomeface.png", &awesomeWidth, &awesomeHeight, &awesomeNrChannels, 0);
    if (awesomeData)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, awesomeWidth, awesomeHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, awesomeData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load texture\n");
        glfwTerminate();
        return -1;
    }
    stbi_image_free(awesomeData);

    // Enable Depth Buffer
    glEnable(GL_DEPTH_TEST);

    // Lots of cubes
    vec3 cubePositions[] = {
        { 0.0f,  0.0f,  0.0f}, 
        { 2.0f,  5.0f, -15.0f}, 
        {-1.5f, -2.2f, -2.5f},  
        {-3.8f, -2.0f, -12.3f},  
        { 2.4f, -0.4f, -3.5f},  
        {-1.7f,  3.0f, -7.5f},  
        { 1.3f, -2.0f, -2.5f},  
        { 1.5f,  2.0f, -2.5f}, 
        { 1.5f,  0.2f, -1.5f}, 
        {-1.3f,  1.0f, -1.5f}
    };

    // Camera stuff
    /*
    vec3 cameraPos = { 0.0f, 0.0f, 0.0f };
    vec3 cameraTarget = { 0.0f, 0.0f, 0.0f };
    vec3 cameraDirection = glm_normalize(cameraPos - cameraTarget);

    vec3 up = { 0.0f, 1.0f, 0.0f };
    vec3 cameraRight = glm_normalize(glm_cross(up, cameraDirection));
    vec3 cameraUp = glm_cross(cameraDirection, cameraRight);

    mat4 view;
    view = glm_lookat(float *eye, float *center, float *up, vec4 *dest)
    */

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Render stuff!!!!
        glClearColor(0.2f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderUse(shaderProgram);
        shaderSetFloat(shaderProgram, "visibility", visibility);

        // --- 3D!!! --- 
        mat4 model;
        glm_mat4_identity(model);
        vec3 modelAxis = { 0.5f, 1.0f, 0.0f };
        glm_rotate(model, (float) glfwGetTime() * glm_rad(50.0f), modelAxis);

        mat4 view;
        glm_mat4_identity(view);
        //vec3 viewTranslation = { view_x, view_y, view_z };
        //glm_translate(view, viewTranslation);

        //const float radius = 10.0f;
        //float camX = sin(glfwGetTime()) * radius;
        //float camZ = cos(glfwGetTime()) * radius;
        //vec3 eye = { camX, 0.0, camZ };
        vec3 center;
        glm_vec3_add(cameraPos, cameraFront, center);
        //vec3 up = { 0.0, 1.0, 0.0 };

        //glm_lookat(cameraPos, center, cameraUp, view);
        lookAt(cameraPos, center, cameraUp, view);

        mat4 projection;
        glm_mat4_identity(projection);
        glm_perspective(glm_rad(fov), (float)windowWidth/(float)windowHeight, 0.1f, 100.0f, projection);

        // Send the matricies to the shaders
        int modelLoc = glGetUniformLocation(shaderProgram->ID, "model");
        int viewLoc = glGetUniformLocation(shaderProgram->ID, "view");
        int projectionLoc = glGetUniformLocation(shaderProgram->ID, "projection");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*) model);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*) view);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float*) projection);
        // --- /3D ---

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, awesomeTexture);

        shaderSetInt(shaderProgram, "ourTexture", 0);
        shaderSetInt(shaderProgram, "awesomeTexture", 1);

        glBindVertexArray(VAO);

        for (unsigned int i = 0; i < 10; i++)
        {
            mat4 cubeModel;
            glm_mat4_identity(cubeModel);
            glm_translate(cubeModel, cubePositions[i]);
            float angle = 20.0f * i;
            vec3 cubeAxis = { 1.0f, 0.3f, 0.5f };
            if (i % 3 == 0)
            {
                angle += glfwGetTime();
            }
            glm_rotate(cubeModel, angle, cubeAxis);
            int modelLoc = glGetUniformLocation(shaderProgram->ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*) cubeModel);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Unbind our vertex array?
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
