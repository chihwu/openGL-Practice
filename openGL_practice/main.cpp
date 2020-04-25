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

const GLint WIDTH {800}, HEIGHT {600};

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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);  // Core Profile indicates that the code won't be backward compatible
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);   // allow forward compatibility
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    // Create OpenGL window and context
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", NULL, NULL);
    
    // Check for window creation failure
    if (!window)
    {
        printf("GLFW window creation failed!");
        // Terminate GLFW
        glfwTerminate();
        return 1;
    }
    
    // Get buffer size information
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
    
    // Set context for GLEW to use
    glfwMakeContextCurrent(window);
    
    // Allow modern extension features
    glewExperimental = GL_TRUE;
    
    if(glewInit() != GLEW_OK) {
        printf("GLEW initialization failed!");
        glfwDestroyWindow(window);
        glfwTerminate();

        return 1;
    }
    
    // set up viewport size
    glViewport(0, 0, bufferWidth, bufferHeight);
    
    // Event loop
    while(!glfwWindowShouldClose(window))
    {
        // Get + Handle user input event
        glfwPollEvents();
        // Clear the screen to black
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
    }
    
    return 0;
}
