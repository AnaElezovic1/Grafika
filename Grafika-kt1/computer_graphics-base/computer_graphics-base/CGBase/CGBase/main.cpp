/**
 * @file main.cpp
 * @author Jovan Ivosevic
 * @brief Base project for Computer Graphics course
 * @version 0.1
 * @date 2022-10-09
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include "shader.hpp"
#include "model.hpp"

const int WindowWidth = 1200;
const int WindowHeight = 700;
const std::string WindowTitle = "Base";
const float TargetFPS = 60.0f;
const float TargetFrameTime = 1.0f / TargetFPS;

/**
 * @brief Keyboard callback function for GLFW. See GLFW docs for details
 *
 * @param window GLFW window context object
 * @param key Triggered key GLFW code
 * @param scancode Triggered key scan code
 * @param action Triggered key action: pressed, released or repeated
 * @param mode Modifiers
 */

struct Input {
    unsigned CurrentDrawing;
    bool DepthTesting;
    bool BackCulling;
};

static void
KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    Input* UserInput = (Input*)glfwGetWindowUserPointer(window);
    bool IsDown = action == GLFW_PRESS || action == GLFW_REPEAT;
    if (IsDown) {
        switch (key) {
        case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
        case GLFW_KEY_D: UserInput->DepthTesting ^= true; break;
        case GLFW_KEY_SPACE: UserInput->CurrentDrawing = 0; break;
        case GLFW_KEY_N: UserInput->CurrentDrawing = 1; break;
        case GLFW_KEY_C: UserInput->BackCulling ^= true; break;
        }
    }
}

/**
 * @brief Error callback function for GLFW. See GLFW docs for details
 *
 * @param error Error code
 * @param description Error message
 */
static void
ErrorCallback(int error, const char* description) {
    std::cerr << "GLFW Error: " << description << std::endl;
}

int main() {
    GLFWwindow* Window = 0;
    if (!glfwInit()) {
        std::cerr << "Failed to init glfw" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwSetErrorCallback(ErrorCallback);

    Window = glfwCreateWindow(WindowWidth, WindowHeight, WindowTitle.c_str(), 0, 0);
    if (!Window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(Window);
    glfwSetKeyCallback(Window, KeyCallback);

    GLenum GlewError = glewInit();
    if (GlewError != GLEW_OK) {
        std::cerr << "Failed to init glew: " << glewGetErrorString(GlewError) << std::endl;
        glfwTerminate();
        return -1;
    }


    Input UserInput = { 0 };
    glfwSetWindowUserPointer(Window, &UserInput);


    Shader Basic("shaders/basic.vert", "shaders/basic.frag");
   
    
    Shader Basic1("shaders/basic.vert", "shaders/basicSun.frag"); 

    Model Anime("ki61/camion.obj");
    if (!Anime.Load()) {
        std::cerr << "Failed to load model" << std::endl;
        glfwTerminate();
        return -1;
    }
    
   


    //pozadina-pasnjak
    float TriangleVertices[] = {
        //x   y      z           r      g     b

      -1.0f, -0.5f, 1.0f,        0.0f, 1.0f, 0.0f,
       1.0f, -0.5f, 1.0f,        0.0f, 1.0f, 0.0f,
      -1.0f, 0.1f,  -0.7f,        0.0f, 1.0f, 0.0f,
     
      1.0f, -0.5f, 1.0f,        0.0f, 1.0f, 0.0f,
      1.0f, 0.1f, -0.7f,        0.0f, 1.0f, 0.0f,
      -1.0f, 0.1f, -0.7f,        0.0f, 1.0f, 0.0f


    };
    unsigned int stride = 6 * sizeof(float);

    unsigned VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleVertices), TriangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    std::vector<float> PlaninaVertices = {


         //gornji deo 
        // X    Y      Z       R    G     B

        -0.7f, 0.9f,   0.0f,  1.2f, 0.8f, 0.0f,
        -0.3f,  0.9f,   0.0f,   1.2f, 0.8f, 0.0f,
        -0.7f, 0.9f,  -0.6f,    1.2f, 0.8f, 0.0f,
        -0.3f,  0.9f,   0.0f,  1.2f, 0.8f, 0.0f,
        -0.3f,  0.9f,  -0.6f,    1.2f, 0.8f, 0.0f,
        -0.7f, 0.9f,  -0.6f,   1.2f, 0.8f, 0.0f,


        //donji deo

        -0.7f, -0.1f, 0.0f,  1.2f, 0.4f, 0.0f,
        -0.3f, -0.1f, 0.0f,   1.2f, 0.4f, 0.0f,
        -0.7f, -0.1f, -0.6f,    1.2f, 0.4f, 0.0f,
        -0.3f, -0.1f, 0.0f,  1.2f, 0.4f, 0.0f,
        -0.3f, -0.1f, -0.6f,    1.2f, 0.4f, 0.0f,
        -0.7f, -0.1f, -0.6f,   1.2f, 0.4f, 0.0f,

        //iza deo

      -0.7f,  -0.1f, -0.7f,   0.2f, 0.8f, 0.0f,
       -0.3f,  -0.1f, -0.7f,  0.2f, 0.8f, 0.0f,
      -0.7f,  0.9f, -0.7f, 0.2f, 0.8f, 0.0f,
       -0.3f,  -0.1f, -0.7f, 0.2f, 0.8f, 0.0f,
       -0.3f,  0.9f, -0.7f,  0.2f, 0.8f, 0.0f,
      -0.7f,  0.9f, -0.7f,   0.2f, 0.8f, 0.0f,

      //ispred deo

      -0.7f,  -0.1f, 0.0f,  0.2f, 0.4f, 0.0f,
       -0.3f,  -0.1f, 0.0f,   0.2f, 0.4f, 0.0f,
      -0.7f,  0.9f, 0.0f, 0.2f, 0.4f, 0.0f,
       -0.3f,  -0.1f, 0.0f,  0.2f, 0.4f, 0.0f,
       -0.3f,  0.9f, 0.0f, 0.2f, 0.4f, 0.0f,
      -0.7f,  0.9f, 0.0f,  0.2f, 0.4f, 0.0f,


      //levo deo

      -0.7f,  -0.1f,  0.0f,  0.2f, 0.7f, 0.0f,
      -0.7f,  -0.1f, -0.7f,   0.2f, 0.7f, 0.0f,
      -0.7f,  0.9f, 0.0f,  0.2f, 0.7f, 0.0f,
      -0.7f,  -0.1f, -0.7f,  0.2f, 0.7f, 0.0f,
      -0.7f,  0.9f, -0.7f,   0.2f, 0.7f, 0.0f,
      -0.7f,  0.9f, 0.0f,  0.2f, 0.7f, 0.0f,

      //desno

       -0.3f,  -0.1f, 0.0f,   0.2f, 0.7f, 0.0f,
       -0.3f,  -0.1f, -0.7f,   0.2f, 0.7f, 0.0f,
       -0.3f,  0.9f, 0.0f,   0.2f, 0.7f, 0.0f,
       -0.3f,  -0.1f, -0.7f,   0.2f, 0.7f, 0.0f,
       -0.3f,  0.9f, -0.7f,   0.2f, 0.7f, 0.0f,
       -0.3f,  0.9f, 0.0f,  0.2f, 0.7f, 0.0f,


      
    };

    // TODO(Jovan): Add indices
    unsigned PlaninaDataStride = 6 * sizeof(float);
    unsigned PlaninaVAO;
    glGenVertexArrays(1, &PlaninaVAO);
    glBindVertexArray(PlaninaVAO);
    unsigned PlaninaVBO;
    glGenBuffers(1, &PlaninaVBO);
    glBindBuffer(GL_ARRAY_BUFFER, PlaninaVBO);
    glBufferData(GL_ARRAY_BUFFER, PlaninaVertices.size() * sizeof(float), PlaninaVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, PlaninaDataStride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, PlaninaDataStride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    


    std::vector<float> SunceVertices = {

        //gornji deo 
        // X    Y      Z       R    G     B

        //donji deo

        0.8f, 0.7f,   0.0f,    0.0f, 1.0f, 0.0f,
        1.0f, 0.7f,   0.0f,    0.0f, 1.0f, 0.0f,
        0.8f, 0.7f,  -0.3f,     0.0f, 1.0f, 0.0f,
        1.0f, 0.7f,   0.0f,    0.0f, 1.0f, 0.0f,
        1.0f, 0.7f,  -0.3f,    0.0f, 1.0f, 0.0f,
        0.8f, 0.7f,  -0.3f,    0.0f, 1.0f, 0.0f,


        //iza deo

         0.8f,  0.7f,  -0.3f,   0.0f, 1.0f, 0.0f,
         1.0f,  0.7f,  -0.3f,   0.0f, 1.0f, 0.0f,
         0.8f,  0.9f,  -0.3f,   0.0f, 1.0f, 0.0f,
         1.0f,  0.7f,  -0.3f,   0.0f, 1.0f, 0.0f,
         1.0f,  0.9f,  -0.3f,   0.0f, 1.0f, 0.0f,
         0.8f,  0.9f,  -0.3f,   0.0f, 1.0f, 0.0f,

        //ispred deo

         0.8f,  0.7f,  0.0f,   0.0f, 1.0f, 0.0f,
         1.0f,  0.7f,  0.0f,  0.0f, 1.0f, 0.0f,
         0.8f,  0.9f,  0.0f,  0.0f, 1.0f, 0.0f,
         1.0f,  0.7f,  0.0f,  0.0f, 1.0f, 0.0f,
         1.0f,  0.9f,  0.0f,  0.0f, 1.0f, 0.0f,
         0.8f,  0.9f,  0.0f,  0.0f, 1.0f, 0.0f,


        //levo deo

         0.8f,  0.7f,   0.0f,  0.0f, 1.0f, 0.0f,
         0.8f,  0.7f,  -0.3f,  0.0f, 1.0f, 0.0f,
         0.8f,  0.9f,   0.0f,  0.0f, 1.0f, 0.0f,
         0.8f,  0.7f,  -0.3f,  0.0f, 1.0f, 0.0f,
         0.8f,  0.9f,  -0.3f,   0.0f, 1.0f, 0.0f,
         0.8f,  0.9f,   0.0f,  0.0f, 1.0f, 0.0f,

        //desno

         1.0f,  0.7f,   0.0f,   0.0f, 1.0f, 0.0f,
         1.0f,  0.7f,  -0.3f,   0.0f, 1.0f, 0.0f,
         1.0f,  0.9f,   0.0f,   0.0f, 1.0f, 0.0f,
         1.0f,  0.7f,  -0.3f,   0.0f, 1.0f, 0.0f,
         1.0f,  0.9f,  -0.3f,   0.0f, 1.0f, 0.0f,
         1.0f,  0.9f,   0.0f,  0.0f, 1.0f, 0.0f

    };

    // TODO(Jovan): Add indices
    unsigned SunceDataStride = 6 * sizeof(float);
    unsigned SunceVAO;
    glGenVertexArrays(1, &SunceVAO);
    glBindVertexArray(SunceVAO);
    unsigned SunceVBO;
    glGenBuffers(1, &SunceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, SunceVBO);
    glBufferData(GL_ARRAY_BUFFER, SunceVertices.size() * sizeof(float), SunceVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, SunceDataStride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, SunceDataStride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

     std::vector<float> KrosnjeVertices = {

         //gornji deo 
         // X    Y      Z       R    G     B
         -0.8f,  0.1f, 0.9f,   0.6f, 1.0f, 0.0f,
         -0.6f,  0.1f, 0.9f,   0.6f, 1.0f, 0.0f,
         -0.6f,  0.1f, 0.6f,   0.6f, 1.0f, 0.0f,
         -0.8f,  0.1f,  0.9f,  0.6f, 1.0f, 0.0f,
         -0.6f,  0.1f, 0.6f,   0.6f, 1.0f, 0.0f,
         -0.8f,  0.1f,0.6f,    0.6f, 1.0f, 0.0f,
       

         //donji deo

         -0.8f, -0.1f, 0.9f,   0.6f, 1.0f, 0.0f,
         -0.6f, -0.1f, 0.9f,   0.6f, 1.0f, 0.0f,
         -0.6f, -0.1f, 0.6f,   0.6f, 1.0f, 0.0f,
         -0.8f, -0.1f, 0.9f,   0.6f, 1.0f, 0.0f,
         -0.6f, -0.1f, 0.6f,   0.6f, 1.0f, 0.0f,
         -0.8f, -0.1f, 0.6f,   0.6f, 1.0f, 0.0f,
        

         //iza deo

         -0.8f, -0.1f, 0.6f,  0.6f, 1.0f, 0.0f,
         -0.6f, -0.1f, 0.6f,  0.6f, 1.0f, 0.0f,
         -0.6f,  0.1f, 0.6f,  0.6f, 1.0f, 0.0f,
         -0.6f,  0.1f, 0.6f,  0.6f, 1.0f, 0.0f,
         -0.8f,  0.1f, 0.6f,  0.6f, 1.0f, 0.0f,
         -0.8f, -0.1f, 0.6f,  0.6f, 1.0f, 0.0f,

         //ispred deo

         -0.8f, -0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
         -0.6f, -0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
         -0.6f,  0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
         -0.8f, -0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
         -0.6f,  0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
         -0.8f,  0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
        

         //levo deo

         -0.8f, -0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
         -0.8f, -0.1f, 0.6f, 0.6f, 1.0f, 0.0f,
         -0.8f,  0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
         -0.8f,  0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
         -0.8f, -0.1f, 0.6f, 0.6f, 1.0f, 0.0f,
         -0.8f,  0.1f, 0.6f, 0.6f, 1.0f, 0.0f,

         //desno

         -0.6f, -0.1f, 0.9f, 0.5f, 1.0f, 0.0f,
         -0.6f, -0.1f, 0.6f, 0.5f, 1.0f, 0.0f,
         -0.6f, 0.1f, 0.9f,  0.5f, 1.0f, 0.0f,
         -0.6f, 0.1f, 0.9f,  0.5f, 1.0f, 0.0f,
         -0.6f, -0.1f, 0.6f, 0.5f, 1.0f, 0.0f,
         -0.6f,  0.1f, 0.6f, 0.5f, 1.0f, 0.0f,


         //gornji deo 
       // X    Y      Z       R    G     B
       -0.8f,  -0.4f, 0.9f, 0.6f, 0.4f, 0.1f,
       -0.6f,  -0.4f, 0.9f, 0.6f, 0.4f, 0.1f,
       -0.6f,  -0.4f, 0.6f,  0.6f, 0.4f, 0.1f,
       -0.8f,  -0.4f,  0.9f, 0.6f, 0.4f, 0.1f,
       -0.6f,  -0.4f, 0.6f,   0.6f, 0.4f, 0.1f,
       -0.8f,  -0.4f,0.6f,  0.6f, 0.4f, 0.1f,


       //donji deo

       -0.8f, -0.1f, 0.9f, 0.6f, 0.4f, 0.1f,
       -0.6f, -0.1f, 0.9f,  0.6f, 0.4f, 0.1f,
       -0.6f, -0.1f, 0.6f,   0.6f, 0.4f, 0.1f,
       -0.8f, -0.1f, 0.9f, 0.6f, 0.4f, 0.1f,
       -0.6f, -0.1f, 0.6f,   0.6f, 0.4f, 0.1f,
       -0.8f, -0.1f, 0.6f,  0.6f, 0.4f, 0.1f,


       //iza deo

       -0.8f, -0.4f, 0.6f,  0.6f, 0.4f, 0.1f,
       -0.6f, -0.4f, 0.6f, 0.6f, 0.4f, 0.1f,
       -0.6f,  -0.1f, 0.6f,0.6f, 0.4f, 0.1f,
       -0.6f,  -0.1f, 0.6f,0.6f, 0.4f, 0.1f,
       -0.8f,  -0.1f, 0.6f, 0.6f, 0.4f, 0.1f,
       -0.8f, -0.4f, 0.6f,  0.6f, 0.4f, 0.1f,

       //ispred deo

       -0.8f, -0.4f, 0.9f, 0.6f, 0.4f, 0.1f,
       -0.6f, -0.4f, 0.9f,  0.6f, 0.4f, 0.1f,
       -0.6f,  -0.1f, 0.9f,0.6f, 0.4f, 0.1f,
       -0.8f, -0.4f, 0.9f, 0.6f, 0.4f, 0.1f,
       -0.6f,  -0.1f, 0.9f,0.6f, 0.4f, 0.1f,
       -0.8f,  -0.1f, 0.9f, 0.6f, 0.4f, 0.1f,


       //levo deo

       -0.8f, -0.4f, 0.9f, 0.6f, 0.4f, 0.1f,
       -0.8f, -0.4f, 0.6f, 0.6f, 0.4f, 0.1f,
       -0.8f,  -0.1f, 0.9f, 0.6f, 0.4f, 0.1f,
       -0.8f,  -0.1f, 0.9f, 0.6f, 0.4f, 0.1f,
       -0.8f, -0.4f, 0.6f, 0.6f, 0.4f, 0.1f,
       -0.8f,  -0.1f, 0.6f, 0.6f, 0.4f, 0.1f,

       //desno

       -0.6f, -0.4f, 0.9f, 0.7f, 0.4f, 0.1f,
       -0.6f, -0.4f, 0.6f, 0.7f, 0.4f, 0.1f,
       -0.6f, -0.1f, 0.9f, 0.7f, 0.4f, 0.1f,
       -0.6f, -0.1f, 0.9f, 0.7f, 0.4f, 0.1f,
       -0.6f, -0.4f, 0.6f, 0.7f, 0.4f, 0.1f,
       -0.6f,  -0.1f, 0.6f, 0.7f, 0.4f, 0.1f,


             //krosnja 2


         //gornji deo 
         // X    Y      Z       R    G     B
             0.8f, 0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
             0.6f, 0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
             0.6f, 0.1f, 0.6f, 0.6f, 1.0f, 0.0f,
             0.8f, 0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
             0.6f, 0.1f, 0.6f, 0.6f, 1.0f, 0.0f,
             0.8f, 0.1f, 0.6f, 0.6f, 1.0f, 0.0f,


             //donji deo

             0.8f, -0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
             0.6f, -0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
             0.6f, -0.1f, 0.6f, 0.6f, 1.0f, 0.0f,
             0.8f, -0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
             0.6f, -0.1f, 0.6f, 0.6f, 1.0f, 0.0f,
             0.8f, -0.1f, 0.6f, 0.6f, 1.0f, 0.0f,


             //iza deo

             0.8f, -0.1f, 0.6f, 0.6f, 1.0f, 0.0f,
             0.6f, -0.1f, 0.6f, 0.6f, 1.0f, 0.0f,
             0.6f, 0.1f, 0.6f, 0.6f, 1.0f, 0.0f,
             0.6f, 0.1f, 0.6f, 0.6f, 1.0f, 0.0f,
             0.8f, 0.1f, 0.6f, 0.6f, 1.0f, 0.0f,
             0.8f, -0.1f, 0.6f, 0.6f, 1.0f, 0.0f,

             //ispred deo

             0.8f, -0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
             0.6f, -0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
             0.6f, 0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
             0.8f, -0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
             0.6f, 0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
             0.8f, 0.1f, 0.9f, 0.6f, 1.0f, 0.0f,


             //levo deo

              0.8f, -0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
              0.8f, -0.1f, 0.6f, 0.6f, 1.0f, 0.0f,
              0.8f, 0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
              0.8f, 0.1f, 0.9f, 0.6f, 1.0f, 0.0f,
              0.8f, -0.1f, 0.6f, 0.6f, 1.0f, 0.0f,
              0.8f, 0.1f, 0.6f, 0.6f, 1.0f, 0.0f,

             //desno

              0.6f, -0.1f, 0.9f, 0.5f, 1.0f, 0.0f,
              0.6f, -0.1f, 0.6f, 0.5f, 1.0f, 0.0f,
              0.6f, 0.1f, 0.9f, 0.5f, 1.0f, 0.0f,
              0.6f, 0.1f, 0.9f, 0.5f, 1.0f, 0.0f,
              0.6f, -0.1f, 0.6f, 0.5f, 1.0f, 0.0f,
              0.6f, 0.1f, 0.6f, 0.5f, 1.0f, 0.0f,

           //stablo2

              //gornji deo 
         //  X    Y      Z       R    G     B
           0.8f, -0.4f, 0.9f, 0.6f, 0.4f, 0.1f,
           0.6f, -0.4f, 0.9f, 0.6f, 0.4f, 0.1f,
           0.6f, -0.4f, 0.6f, 0.6f, 0.4f, 0.1f,
           0.8f, -0.4f, 0.9f, 0.6f, 0.4f, 0.1f,
           0.6f, -0.4f, 0.6f, 0.6f, 0.4f, 0.1f,
           0.8f, -0.4f, 0.6f, 0.6f, 0.4f, 0.1f,


           //donji deo

           0.8f, -0.1f, 0.9f, 0.6f, 0.4f, 0.1f,
           0.6f, -0.1f, 0.9f, 0.6f, 0.4f, 0.1f,
           0.6f, -0.1f, 0.6f, 0.6f, 0.4f, 0.1f,
           0.8f, -0.1f, 0.9f, 0.6f, 0.4f, 0.1f,
           0.6f, -0.1f, 0.6f, 0.6f, 0.4f, 0.1f,
           0.8f, -0.1f, 0.6f, 0.6f, 0.4f, 0.1f,


           //iza deo

           0.8f, -0.4f, 0.6f, 0.6f, 0.4f, 0.1f,
           0.6f, -0.4f, 0.6f, 0.6f, 0.4f, 0.1f,
           0.6f, -0.1f, 0.6f, 0.6f, 0.4f, 0.1f,
           0.6f, -0.1f, 0.6f, 0.6f, 0.4f, 0.1f,
           0.8f, -0.1f, 0.6f, 0.6f, 0.4f, 0.1f,
           0.8f, -0.4f, 0.6f, 0.6f, 0.4f, 0.1f,

           //ispred deo

           0.8f, -0.4f, 0.9f, 0.6f, 0.4f, 0.1f,
           0.6f, -0.4f, 0.9f, 0.6f, 0.4f, 0.1f,
           0.6f, -0.1f, 0.9f, 0.6f, 0.4f, 0.1f,
           0.8f, -0.4f, 0.9f, 0.6f, 0.4f, 0.1f,
           0.6f, -0.1f, 0.9f, 0.6f, 0.4f, 0.1f,
           0.8f, -0.1f, 0.9f, 0.6f, 0.4f, 0.1f,


           //levo deo

           0.8f, -0.4f, 0.9f, 0.6f, 0.4f, 0.1f,
           0.8f, -0.4f, 0.6f, 0.6f, 0.4f, 0.1f,
           0.8f, -0.1f, 0.9f, 0.6f, 0.4f, 0.1f,
           0.8f, -0.1f, 0.9f, 0.6f, 0.4f, 0.1f,
           0.8f, -0.4f, 0.6f, 0.6f, 0.4f, 0.1f,
           0.8f, -0.1f, 0.6f, 0.6f, 0.4f, 0.1f,

           //desno

           0.6f, -0.4f, 0.9f, 0.7f, 0.4f, 0.1f,
           0.6f, -0.4f, 0.6f, 0.7f, 0.4f, 0.1f,
           0.6f, -0.1f, 0.9f, 0.7f, 0.4f, 0.1f,
           0.6f, -0.1f, 0.9f, 0.7f, 0.4f, 0.1f,
           0.6f, -0.4f, 0.6f, 0.7f, 0.4f, 0.1f,
           0.6f, -0.1f, 0.6f, 0.7f, 0.4f, 0.1f,


       
         
     };

     // TODO(Jovan): Add indices
     unsigned CubeDataStride = 6 * sizeof(float);
     unsigned CubeVAO;
     glGenVertexArrays(1, &CubeVAO);
     glBindVertexArray(CubeVAO);
     unsigned CubeVBO;
     glGenBuffers(1, &CubeVBO);
     glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
     glBufferData(GL_ARRAY_BUFFER, KrosnjeVertices.size() * sizeof(float), KrosnjeVertices.data(), GL_STATIC_DRAW);
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, CubeDataStride, (void*)0);
     glEnableVertexAttribArray(0);
     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, CubeDataStride, (void*)(3 * sizeof(float)));
     glEnableVertexAttribArray(1);
     glBindBuffer(GL_ARRAY_BUFFER, 0);
     glBindVertexArray(0);

    
     
     glViewport(0, 0, WindowWidth, WindowHeight);
     //perspective(FOV ugao, aspect ratio prozora, prednja odsjecna ravan i zadnja odsjecna ravan)
                                        //fov-45.0f                                udaljenost blize i dalje ravni
     glm::mat4 Projection = glm::perspective(45.0f, WindowWidth / (float)WindowHeight, 0.1f, 100.0f);
     //lookAt(pozicija kamere, tacka u koju kamera gleda, kako je kamera rotirana po osi definisanoj sa prethodne dvije tacke)
     glm::mat4 View = glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
     //matrica modela
     glm::mat4 Model(1.0f);

     glm::mat4 m(1.0f);





    float FrameStartTime = glfwGetTime();
    float FrameEndTime = glfwGetTime();
    float dt = FrameEndTime - FrameStartTime;
    while (!glfwWindowShouldClose(Window)) {
        //deph 
        if (UserInput.DepthTesting) {
            glEnable(GL_DEPTH_TEST);
        }
        else {
            glDisable(GL_DEPTH_TEST);
        }

       //back culling
        if (UserInput.BackCulling) {

            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            glFrontFace(GL_CCW);
        }
        else {

            glDisable(GL_CULL_FACE);
        }
        glfwPollEvents();
        
        //glClearColor(0.0 , 0.7, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            

        //pozadina za nebo gore
            glClearColor(0.4, 0.7, 1.0, 1.0);

            FrameStartTime = glfwGetTime();
            glUseProgram(Basic.GetId());

            
            //celokupna scena 
            
             Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, -2.0f));
             

            //static float angle = 1.0f;
            //Model = glm::rotate(Model, glm::radians(angle), glm::vec3(1.0f, 1.0f, 1.0));
            Basic.SetUniform4m("uMVP", Projection* View* Model);
            
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindVertexArray(0);
            glBindVertexArray(CubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 144);
            glBindVertexArray(PlaninaVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);


            //dan i noc je ovdee u ovom switch-case
            switch (UserInput.CurrentDrawing) {
            case 0: {
                glClearColor(0.4, 0.7, 1.0, 1.0);

                glUseProgram(Basic1.GetId());


                glBindVertexArray(SunceVAO);


                float R = abs(sin(glfwGetTime())); //Absolutna vrijednost sinusa trenutnog vremena
                float G = 0.2;
                int colorOffsetLocation = glGetUniformLocation(Basic1.GetId(), "offset"); //Nadji adresu uniforme

                glUniform3f(colorOffsetLocation, R, G, 0);
                Basic1.SetUniform4m("uMVP", Projection * View * Model);

                glDrawArrays(GL_TRIANGLES, 0, 36);
                glBindVertexArray(0);




            }; break;
            case 1: {
                glClearColor(0.0, 0.2, 1.0, 1.0);
                glUseProgram(Basic1.GetId());

                unsigned int colorOffsetLocation = glGetUniformLocation(Basic1.GetId(), "offset"); //Nadji adresu uniforme
                glUniform3f(colorOffsetLocation, 0.3, 0.5, 0.6);
                Basic1.SetUniform4m("uMVP", Projection * View * Model);

                ;
                glBindVertexArray(SunceVAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                glBindVertexArray(0);


            }; break;
            }
            

            glUseProgram(Basic.GetId());
            
            //ovde je model celokupnog prikaza sa planinama travom....
            Model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));
            Model = glm::scale(Model, glm::vec3(2.3));
            Basic.SetModel(Model);
            //Anime.Render();

            glUseProgram(Basic.GetId());

            //ovde se prikazuje model kamiona
            m = glm::translate(glm::mat4(1.0f), glm::vec3(1.0, -0.9, -0.5));
            m = glm::scale(m, glm::vec3(6.3));
            Basic.SetModel(m);
            Anime.Render();
           



        glUseProgram(0);
        glfwSwapBuffers(Window);

        // NOTE(Jovan): Frame stabilization
        FrameEndTime = glfwGetTime();
        dt = FrameEndTime - FrameStartTime;
        if (dt < TargetFPS) {
            int DeltaMS = (int)((TargetFrameTime - dt) * 1e3f);
            std::this_thread::sleep_for(std::chrono::milliseconds(DeltaMS));
            FrameEndTime = glfwGetTime();
        }
        dt = FrameEndTime - FrameStartTime;
    }

    glfwTerminate();
    return 0;
}



