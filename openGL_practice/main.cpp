//
//  main.cpp
//  openGL_practice
//
//  Created by Chihyung Wu on 4/19/20.
//  Copyright © 2020 Chihyung Wu. All rights reserved.
//

#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "filesystem.h"

#include "shader_m.h"
#include "camera.h"
#include "model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Define main function
int main()
{
    // Initialize GLFW
    if (!glfwInit()) {
        printf("GLFW initialization failed!");
        glfwTerminate();
        
        return 1;
    }
    
    // Define version and compatibility settings
    // Since the focus of this website is on OpenGL version 3.3, we'd like to tell GLFW that 3.3 is the OpenGL version we want to use. This way GLFW can make the proper arrangements when creating the OpenGL context.
    // This ensures that when an user does not have the proper OpenGL version GLFW fails to run
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);  // Core Profile indicates that the code won't be backward compatible and we will only access to a smaller subset of OpenGL features
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);   // This line is required on Mac OS and it allows forward compatibility
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    // Create OpenGL window and context
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    
    // Check for window creation failure
    if (!window)
    {
        printf("GLFW window creation failed!");
        // Terminate GLFW
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);  // Here we tell GLFW to make the context of our window the main context on the current thread
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // when the window is first displayed, framebuffer_size_callback will be called as well as when every time the window is resized. We register the callback functions after we've created the window and before the game loop in initiated.
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if(glewInit() != GLEW_OK) {
        printf("GLEW initialization failed!");
        glfwDestroyWindow(window);
        glfwTerminate();

        return 1;
    }
    
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);
    
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    
    // build and compile shaders
    // -------------------------
    Shader ourShader("model_loading.vs", "model_loading.fs");
    
    // load models
    // -----------
    Model ourModel(FileSystem::getPath("LibertStatue.obj"));

    // this while loop is called the render loop
    while(!glfwWindowShouldClose(window))  // The glfwWindowShouldClose() function checks at the start of each loop ireration if GLFW has been instructed to close. If a window should be closed is based on the returned value from the function glfwSetwindowShouldClose()
    {
        // IMPORTANT: all the rendering commands should be in the render loop since we want to execute all the rednering commands in each iteration of the loop.
        
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // input
        // -----
        processInput(window);
        
        // render
        // ------
        // At the start of each render iteration we always want to clear the screen otherwise we would still see the results from the previous iteration
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f); // Whenever we call glClear and clear the color buffer, the entire color buffer will be filled with the color as configured by glClearColor(), so this should be set before the glClear() is called
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // At the start of each render iteration we always want to clear the screen otherwise we would still see the results from the previous iteration
        
        // don't forget to enable shader before setting uniforms
        ourShader.use();
        
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        
        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));    // it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);
        
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);  // this will swap the color buffer (a large buffer that contains color values for each pixel in GLFW's window) that has been used to draw in during this iteration and show it as output to the screen. (Double buffers are used to avoid flickering issues.)
        glfwPollEvents();  // this checks if any events are triggered (like keyboard input or mouse movement events), updates the window state, and calls the corresponding functions such as callback methods.
    }
    
    glfwTerminate(); // as soon as we exit the render loop we would like to properly clean/delete all resources thar were allocated using this glfwTerminate()
    
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // if it not pressed glfwGetKey returns GLFW_RELEASE
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// Whenever the window changes in size, GLFW calls this function and fills in the proper arguments for you to process
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and height will be significantly larger than the original input values for retina displays.
    // The first two parameters of glViewport set the location of the lower left corner of the window. The third and fourth parameter set the width and height of the rendering window in pixels.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
