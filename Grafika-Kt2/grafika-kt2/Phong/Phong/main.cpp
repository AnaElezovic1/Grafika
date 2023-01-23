
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <thread>
#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "texture.hpp"

float
Clamp(float x, float min, float max) {
    return x < min ? min : x > max ? max : x;
}

int WindowWidth = 1800;
int WindowHeight = 1000;
const float TargetFPS = 60.0f;
const std::string WindowTitle = "Suma";


struct Input {
    bool DepthTesting;
    bool BackCulling;
    
    
    bool MoveLeft;
    bool MoveRight;
    bool MoveUp;
    bool MoveDown;
    bool LookLeft;
    bool LookRight;
    bool LookUp;
    bool LookDown;
    bool GoUp;
    bool GoDown;
};

struct EngineState {
    Input* mInput;
    Camera* mCamera;
    unsigned mShadingMode;
    bool mDrawDebugLines;
    float mDT;
};

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

/**
 * @brief Keyboard callback function for GLFW. See GLFW docs for details
 *
 * @param window GLFW window context object
 * @param key Triggered key GLFW code
 * @param scancode Triggered key scan code
 * @param action Triggered key action: pressed, released or repeated
 * @param mode Modifiers
 */
static void
KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    EngineState* State = (EngineState*)glfwGetWindowUserPointer(window);
    Input* UserInput = State->mInput;
    bool IsDown = action == GLFW_PRESS || action == GLFW_REPEAT;
    switch (key) {
    case GLFW_KEY_A: UserInput->MoveLeft = IsDown; break;
    case GLFW_KEY_D: UserInput->MoveRight = IsDown; break;
    case GLFW_KEY_W: UserInput->MoveUp = IsDown; break;
    case GLFW_KEY_S: UserInput->MoveDown = IsDown; break;

    case GLFW_KEY_RIGHT: UserInput->LookLeft = IsDown; break;
    case GLFW_KEY_LEFT: UserInput->LookRight = IsDown; break;
    case GLFW_KEY_UP: UserInput->LookUp = IsDown; break;
    case GLFW_KEY_DOWN: UserInput->LookDown = IsDown; break;

    case GLFW_KEY_SPACE: UserInput->GoUp = IsDown; break;
    case GLFW_KEY_C: UserInput->GoDown = IsDown; break;

    case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;

    case GLFW_KEY_L: {
        if (IsDown) {
            State->mDrawDebugLines ^= true; break;
        }
    } break;

   
    
    }
}

static void
FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
   
    glViewport(0, 0, width, height);
}

/**
 * @brief Updates engine state based on input
 * 
 * @param state EngineState
 */
static void
HandleInput(EngineState* state) {
    Input* UserInput = state->mInput;
    Camera* FPSCamera = state->mCamera;
    if (UserInput->MoveLeft) FPSCamera->Move(-1.0f, 0.0f, state->mDT);
    if (UserInput->MoveRight) FPSCamera->Move(1.0f, 0.0f, state->mDT);
    if (UserInput->MoveDown) FPSCamera->Move(0.0f, -1.0f, state->mDT);
    if (UserInput->MoveUp) FPSCamera->Move(0.0f, 1.0f, state->mDT);

    if (UserInput->LookLeft) FPSCamera->Rotate(1.0f, 0.0f, state->mDT);
    if (UserInput->LookRight) FPSCamera->Rotate(-1.0f, 0.0f, state->mDT);
    if (UserInput->LookDown) FPSCamera->Rotate(0.0f, -1.0f, state->mDT);
    if (UserInput->LookUp) FPSCamera->Rotate(0.0f, 1.0f, state->mDT);

    if (UserInput->GoUp) FPSCamera->UpDown(1);
    if (UserInput->GoDown) FPSCamera->UpDown(-1);
}

/**
 * @brief Draws flattened cubes
 *
 * @param vao - Cube VAO
 * @param shader - Shader
 */
static void DrawTrava(unsigned vao, const Shader& shader, unsigned diffuse,  unsigned specular) {
    glUseProgram(shader.GetId());
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specular);

    float Size = 4.0f;
    for (int i = -2; i < 4; ++i) {
        for (int j = -2; j < 4; ++j) {
            glm::mat4 Model(1.0f);
            Model = glm::translate(Model, glm::vec3(i * Size, 0.0f, j * Size));
            Model = glm::scale(Model, glm::vec3(Size, 0.1f, Size));
            shader.SetModel(Model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
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

    Window = glfwCreateWindow(WindowWidth, WindowHeight, WindowTitle.c_str(), 0, 0);
    if (!Window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(Window);

    GLenum GlewError = glewInit();
    if (GlewError != GLEW_OK) {
        std::cerr << "Failed to init glew: " << glewGetErrorString(GlewError) << std::endl;
        glfwTerminate();
        return -1;
    }

    EngineState State = { 0 };
    Camera FPSCamera;
    Input UserInput = { 0 };
    State.mCamera = &FPSCamera;
    State.mInput = &UserInput;

    glfwSetWindowUserPointer(Window, &State);

    glfwSetErrorCallback(ErrorCallback);
    glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);
    glfwSetKeyCallback(Window, KeyCallback);

    glViewport(0.0f, 0.0f, WindowWidth, WindowHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    //Difuzne strukture
    unsigned TravaDiffuseTexture = Texture::LoadImageToTexture("res/trava.jpg");
    unsigned DrvoDiffuseTexture = Texture::LoadImageToTexture("res/drvo.jpg");
    unsigned KrosnjaDiffuseTexture = Texture::LoadImageToTexture("res/krosnja.jpeg");
    unsigned PlaninaDiffuseTexture = Texture::LoadImageToTexture("res/planina.jpg");
    //unsigned Trava1DiffuseTexture = Texture::LoadImageToTexture("res/trava1.jpg");
    unsigned SunceDiffuseTexture = Texture::LoadImageToTexture("res/sunce.jpg");
    unsigned MesecDiffuseTexture = Texture::LoadImageToTexture("res/mesec.jpg");

    //spekularne strukture
    unsigned TravaSpecularTexture = Texture::LoadImageToTexture("res/trava2_s.jpg");
    //unsigned Trava1SpecularTexture = Texture::LoadImageToTexture("res/trava1_s.jpg");
    


    std::vector<float> CubeVertices = {
        // X     Y     Z     NX    NY    NZ    U     V    FRONT SIDE
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // L D
         0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // R D
        -0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // L U
         0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // R D
         0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // R U
        -0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // L U
                                                        // LEFT SIDE
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // L D
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
        -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
        -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // R U
        -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
                                                        // RIGHT SIDE
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // L D
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // R D
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // R U
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // L U
                                                        // BOTTOM SIDE
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // L D
         0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // R D
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // L U
         0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // R D
         0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // R U
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // L U
                                                        // TOP SIDE
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // L D
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // R D
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // L U
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // R D
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // R U
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // L U
                                                        // BACK SIDE
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // L D
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // R D
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // L U
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // R D
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // R U
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // L U
    };

    unsigned CubeVAO;
    glGenVertexArrays(1, &CubeVAO);
    glBindVertexArray(CubeVAO);
    unsigned CubeVBO;
    glGenBuffers(1, &CubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
    glBufferData(GL_ARRAY_BUFFER, CubeVertices.size() * sizeof(float), CubeVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Model Fox("res/low-poly-fox/low-poly-fox.obj");
    if (!Fox.Load()) {
        std::cerr << "Failed to load fox\n";
        glfwTerminate();
        return -1;
    }


    // NOTE(Jovan): Phong shader with material and texture support
    Shader PhongShaderMaterialTexture("shaders/basic.vert", "shaders/phong_material_texture.frag");
    Shader ColorShader("shaders/color.vert", "shaders/color.frag");
    glUseProgram(PhongShaderMaterialTexture.GetId());
   
    //Kamen1
    PhongShaderMaterialTexture.SetUniform3f("uKamenLight.Ka", glm::vec3(0.4, 0.1, 0.5));
    PhongShaderMaterialTexture.SetUniform3f("uKamenLight.Kd", glm::vec3(0.4, 0.1, 0.5));
    PhongShaderMaterialTexture.SetUniform3f("uKamenLight.Ks", glm::vec3(3));

    //Kamen2
    PhongShaderMaterialTexture.SetUniform3f("uKamenLight1.Ka", glm::vec3(0.4, 0.1, 0.5));
    PhongShaderMaterialTexture.SetUniform3f("uKamenLight1.Kd", glm::vec3(0.4, 0.1, 0.5));
    PhongShaderMaterialTexture.SetUniform3f("uKamenLight1.Ks", glm::vec3(3));

    //Kamen3
    PhongShaderMaterialTexture.SetUniform3f("uKamenLight2.Ka", glm::vec3(0.4, 0.1, 0.5));
    PhongShaderMaterialTexture.SetUniform3f("uKamenLight2.Kd", glm::vec3(0.4, 0.1, 0.5));
    PhongShaderMaterialTexture.SetUniform3f("uKamenLight2.Ks", glm::vec3(3));

    //Kamen4
    PhongShaderMaterialTexture.SetUniform3f("uKamenLight3.Ka", glm::vec3(0.4, 0.1, 0.5));
    PhongShaderMaterialTexture.SetUniform3f("uKamenLight3.Kd", glm::vec3(0.4, 0.1, 0.5));
    PhongShaderMaterialTexture.SetUniform3f("uKamenLight3.Ks", glm::vec3(3));

    //Kamen5
    PhongShaderMaterialTexture.SetUniform3f("uKamenLight4.Ka", glm::vec3(0.4, 0.1, 0.5));
    PhongShaderMaterialTexture.SetUniform3f("uKamenLight4.Kd", glm::vec3(0.4, 0.1, 0.5));
    PhongShaderMaterialTexture.SetUniform3f("uKamenLight4.Ks", glm::vec3(3));

    //Sunce
    PhongShaderMaterialTexture.SetUniform3f("uSunceLight.Ka", glm::vec3(0.4, 0.1, 0.5));
    PhongShaderMaterialTexture.SetUniform3f("uSunceLight.Kd", glm::vec3(0.4, 0.1, 0.5));
    PhongShaderMaterialTexture.SetUniform3f("uSunceLight.Ks", glm::vec3(3));

    //Mesec
    PhongShaderMaterialTexture.SetUniform3f("uMesecLight.Ka", glm::vec3(0.1, 0.2, 0.9));
    PhongShaderMaterialTexture.SetUniform3f("uMesecLight.Kd", glm::vec3(0.1, 0.2, 0.9));
    PhongShaderMaterialTexture.SetUniform3f("uMesecLight.Ks", glm::vec3(3));


    //svetlo za dan reflektorno
    PhongShaderMaterialTexture.SetUniform3f("uReflektorLight1.Ka", glm::vec3(1.4, 0.1, 0.5));
    PhongShaderMaterialTexture.SetUniform3f("uReflektorLight1.Kd", glm::vec3(1.4, 0.1, 0.5));
    PhongShaderMaterialTexture.SetUniform3f("uReflektorLight1.Ks", glm::vec3(3));
    //koliko je svetlo udaljeno
    PhongShaderMaterialTexture.SetUniform1f("uReflektorLight1.Kc", 1.0f);
    PhongShaderMaterialTexture.SetUniform1f("uReflektorLight1.Kl", 0.0002f);
    PhongShaderMaterialTexture.SetUniform1f("uReflektorLight1.Kq", 0.0002f);
    PhongShaderMaterialTexture.SetUniform1f("uReflektorLight1.InnerCutOff", glm::cos(glm::radians(5.0f)));
    PhongShaderMaterialTexture.SetUniform1f("uReflektorLight1.OuterCutOff", glm::cos(glm::radians(10.0f)));

    //svetlo za dan reflektorno za noc
    PhongShaderMaterialTexture.SetUniform3f("uReflektorLight2.Ka", glm::vec3(0.0, 0.50, 0.74));
    PhongShaderMaterialTexture.SetUniform3f("uReflektorLight2.Kd", glm::vec3(0.0, 0.50, 0.74));
    PhongShaderMaterialTexture.SetUniform3f("uReflektorLight2.Ks", glm::vec3(1));
    PhongShaderMaterialTexture.SetUniform1f("uReflektorLight2.Kc", 1.0f);
    PhongShaderMaterialTexture.SetUniform1f("uReflektorLight2.Kl", 0.0002f);
    PhongShaderMaterialTexture.SetUniform1f("uReflektorLight2.Kq", 0.0002f);
    PhongShaderMaterialTexture.SetUniform1f("uReflektorLight2.InnerCutOff", glm::cos(glm::radians(5.0f)));
    PhongShaderMaterialTexture.SetUniform1f("uReflektorLight2.OuterCutOff", glm::cos(glm::radians(10.0f)));
    
    // NOTE(Jovan): Diminishes the light's diffuse component by half, tinting it slightly red
    PhongShaderMaterialTexture.SetUniform1i("uMaterial.Kd", 0);
    // NOTE(Jovan): Makes the object really shiny
    PhongShaderMaterialTexture.SetUniform1i("uMaterial.Ks", 1);
    PhongShaderMaterialTexture.SetUniform1f("uMaterial.Shininess", 32.0f);
    glUseProgram(0);

    



    glm::mat4 Projection = glm::perspective(45.0f, WindowWidth / (float)WindowHeight, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(FPSCamera.GetPosition(), FPSCamera.GetTarget(), FPSCamera.GetUp());
    
    //model za lisicu
    glm::mat4 ModelMatrix(1.0f);
    
    // NOTE(Jovan): Current angle around Y axis, with regards to XZ plane at which the point light is situated at
    //float Angle = 0.0f;
    // NOTE(Jovan): Distance of point light from center of rotation
    float Distance = 5.0f;
    float TargetFrameTime = 1.0f / TargetFPS;
    float StartTime = glfwGetTime();
    float EndTime = glfwGetTime();

    //model scene
    glm::mat4 model_matrix(1.0f);

    bool is_day = true;

   

    // NOTE(Jovan): Currently used shader
    Shader* CurrentShader = &PhongShaderMaterialTexture;
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
    while (!glfwWindowShouldClose(Window)) {
        glfwPollEvents();
        HandleInput(&State);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
      
        // NOTE(Jovan): In case of window resize, update projection. Bit bad for performance to do it every iteration.
        // If laggy, remove this line
        Projection = glm::perspective(45.0f, WindowWidth / (float)WindowHeight, 0.1f, 100.0f);
        View = glm::lookAt(FPSCamera.GetPosition(), FPSCamera.GetTarget(), FPSCamera.GetUp());
        StartTime = glfwGetTime();
       
        

        //prikaz modela 
        glUseProgram(CurrentShader->GetId());
        CurrentShader->SetProjection(Projection);
        CurrentShader->SetView(View);
        CurrentShader->SetUniform3f("uViewPos", FPSCamera.GetPosition());
        

        DrawTrava(CubeVAO, *CurrentShader, TravaDiffuseTexture, TravaSpecularTexture);


        // NOTE(Jovan): Rotate point light around 0, 0, -2
       // glm::vec3 PointLightPosition(Distance * cos(Angle), 2.0f, -2.0f + Distance * sin(Angle));
        //CurrentShader->SetUniform3f("uPointLight.Position", PointLightPosition);
        //Angle += State.mDT;

        
        //lisica model
        ModelMatrix = glm::mat4(1.0f);
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1.0f, 0.7f, 9.0f));
        CurrentShader->SetModel(ModelMatrix);
        Fox.Render();

        if (glfwGetKey(Window, GLFW_KEY_N) == GLFW_PRESS)
        {
            is_day = false;
        }
        if (glfwGetKey(Window, GLFW_KEY_M) == GLFW_PRESS)
        {
            is_day = true;
        }

        if (is_day) {
            glClearColor(0.53f, 0.81f, 0.98f, 1.0f);

            glm::vec3 point_light_position_sun(-1.0f, 6.7f, 7.0f); 
            
            CurrentShader->SetUniform3f("uDirLight.Direction", glm::vec3(0, -0.1, 0));
            CurrentShader->SetUniform3f("uDirLight.Ka", glm::vec3(0.8, 0.8, 0.3));
            CurrentShader->SetUniform3f("uDirLight.Kd", glm::vec3(0.8, 0.8, 0.3));
            CurrentShader->SetUniform3f("uDirLight.Ks", glm::vec3(0.88, 1.0, 0.0));


            CurrentShader->SetUniform3f("uReflektorLight1.Position", point_light_position_sun);
            CurrentShader->SetUniform3f("uReflektorLight1.Direction", glm::vec3(5.5, -20, 5.0));
            /*
            glUseProgram(ColorShader.GetId());
            ColorShader.SetProjection(Projection);
            ColorShader.SetView(View);

            //sunce
           
            model_matrix = glm::mat4(1.0f);
            model_matrix = glm::translate(model_matrix, point_light_position_sun);
            model_matrix = glm::scale(model_matrix, glm::vec3(3,2,2));
            ColorShader.SetModel(model_matrix); 
            unsigned int colorOffsetLocation = glGetUniformLocation(ColorShader.GetId(), "offset"); //Nadji adresu uniforme
            glUniform3f(colorOffsetLocation, 0.9, 1.0, 0.0);
            ColorShader.SetUniform4m("uMVP", Projection * View * model_matrix);

            */

            CurrentShader->SetUniform3f("uSunceLight.Position", point_light_position_sun);
            CurrentShader->SetUniform1f("uSunceLight.Kc", 0.1 / abs(sin(StartTime)));
            CurrentShader->SetUniform1f("uSunceLight.Kl", 0.1 / abs(sin(StartTime)));
            CurrentShader->SetUniform1f("uSunceLight.Kq", 1.0 / abs(sin(StartTime)));
           
            model_matrix = glm::mat4(1.0f);
            model_matrix = glm::translate(model_matrix, point_light_position_sun);
            model_matrix = glm::scale(model_matrix, glm::vec3(1));
            CurrentShader->SetModel(model_matrix);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, SunceDiffuseTexture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, SunceDiffuseTexture);
            glBindVertexArray(CubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);


            glBindVertexArray(CubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);
        }

        else {
        
            glm::vec3 point_light_position_sun(-1.0f, 6.7f, 7.0f);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glUseProgram(PhongShaderMaterialTexture.GetId());
           
            CurrentShader->SetUniform3f("uDirLight.Direction", glm::vec3(0, -0.1, 0));
            //ambijentno
            CurrentShader->SetUniform3f("uDirLight.Ka", glm::vec3(0.1, 0.2, 0.4));
            //difuzno
            CurrentShader->SetUniform3f("uDirLight.Kd", glm::vec3(0.1, 0.2, 0.4));
            //reflektivno 
            CurrentShader->SetUniform3f("uDirLight.Ks", glm::vec3(0.6,0.5,0.6));
          

            CurrentShader->SetUniform3f("uReflektorLight2.Position", point_light_position_sun);
            CurrentShader->SetUniform3f("uReflektorLight2.Direction", glm::vec3(5.5, -20, 5.0));
            
            /*
            glUseProgram(ColorShader.GetId());
            ColorShader.SetProjection(Projection);
            ColorShader.SetView(View);

            //sunce
            
            model_matrix = glm::mat4(1.0f);
            model_matrix = glm::translate(model_matrix, point_light_position_sun);
            model_matrix = glm::scale(model_matrix, glm::vec3(3, 2, 2));
            ColorShader.SetModel(model_matrix);

            unsigned int colorOffsetLocation = glGetUniformLocation(ColorShader.GetId(), "offset"); //Nadji adresu uniforme
            glUniform3f(colorOffsetLocation, 0.3, 0.5, 0.6);
            ColorShader.SetUniform4m("uMVP", Projection * View * model_matrix);*/
            

            CurrentShader->SetUniform3f("uMesecLight.Position", point_light_position_sun);
            CurrentShader->SetUniform1f("uMesecLight.Kc", 0.1 / abs(sin(StartTime)));
            CurrentShader->SetUniform1f("uMesecLight.Kl", 0.1 / abs(sin(StartTime)));
            CurrentShader->SetUniform1f("uMesecLight.Kq", 1.0 / abs(sin(StartTime)));

            model_matrix = glm::mat4(1.0f);
            model_matrix = glm::translate(model_matrix, point_light_position_sun);
            model_matrix = glm::scale(model_matrix, glm::vec3(1));
            CurrentShader->SetModel(model_matrix);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, MesecDiffuseTexture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, MesecDiffuseTexture);
            glBindVertexArray(CubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

            
            glBindVertexArray(CubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);



        }

        glUseProgram(PhongShaderMaterialTexture.GetId());
        //Stablo1
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(-4.0f, 1.0f, 1.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(1, 2, 1));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, DrvoDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, DrvoDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        //Stablo2
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(-1.0f, 1.0f, 4.5f));
        model_matrix = glm::scale(model_matrix, glm::vec3(1, 2, 1));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, DrvoDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, DrvoDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        //Stablo3
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(-5.0f, 1.0f, 6.5f));
        model_matrix = glm::scale(model_matrix, glm::vec3(1, 2, 1));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, DrvoDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, DrvoDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        //Stablo4
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(6.6f, 1.0f, 6.5f));
        model_matrix = glm::scale(model_matrix, glm::vec3(1, 2, 1));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, DrvoDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, DrvoDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        //Stablo5
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(9.6f, 1.0f, 4.5f));
        model_matrix = glm::scale(model_matrix, glm::vec3(1, 2, 1));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, DrvoDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, DrvoDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);


        //Stablo6
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(10.6f, 1.0f, 9.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(1, 2, 1));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, DrvoDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, DrvoDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);


        //Stablo7
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(5.6f, 1.0f, 10.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(1, 2, 1));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, DrvoDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, DrvoDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        //Stablo8
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(-6.6f, 1.0f, -1.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(1, 2, 1));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, DrvoDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, DrvoDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);



        //krosnja1
       //glm::vec3 point_light_position_sun(10, 10, 0);
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(-4.0f, 2.0f, 1.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(1.5));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, KrosnjaDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, KrosnjaDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        //krosnja2
      //glm::vec3 point_light_position_sun(10, 10, 0);
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(-1.0f, 2.0f, 4.5f));
        model_matrix = glm::scale(model_matrix, glm::vec3(1.5));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, KrosnjaDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, KrosnjaDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        //krosnja3
   //glm::vec3 point_light_position_sun(10, 10, 0);
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(-5.0f, 2.0f, 6.5f));
        model_matrix = glm::scale(model_matrix, glm::vec3(1.5));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, KrosnjaDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, KrosnjaDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);


        //krosnja4
   //glm::vec3 point_light_position_sun(10, 10, 0);
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(6.6f, 2.0f, 6.5f));
        model_matrix = glm::scale(model_matrix, glm::vec3(1.5));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, KrosnjaDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, KrosnjaDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);


        //krosnja5
   //glm::vec3 point_light_position_sun(10, 10, 0);
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(9.6f, 2.0f, 4.5f));
        model_matrix = glm::scale(model_matrix, glm::vec3(1.5));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, KrosnjaDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, KrosnjaDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        //krosnja6
//glm::vec3 point_light_position_sun(10, 10, 0);
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(10.6f, 2.0f, 9.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(1.5));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, KrosnjaDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, KrosnjaDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);


        //krosnja7
//glm::vec3 point_light_position_sun(10, 10, 0);
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(5.6f, 2.0f, 10.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(1.5));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, KrosnjaDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, KrosnjaDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);


        //krosnja8
//glm::vec3 point_light_position_sun(10, 10, 0);
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(-6.6f, 2.0f, -1.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(1.5));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, KrosnjaDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, KrosnjaDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);


        //planina
        model_matrix = glm::mat4(1.0f);
        /*model_matrix = glm::translate(model_matrix, glm::vec3(7.6f, -3.1f, -6.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(9, 9, 9));
        model_matrix = glm::rotate(model_matrix, glm::radians(45.0f), glm::vec3(0, 1, 0));
        model_matrix = glm::rotate(model_matrix, glm::radians(90.0f), glm::vec3(1, 1, 0));*/

        model_matrix = glm::translate(model_matrix, glm::vec3(7.6f, 3.1f, -6.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(7, 7, 4));

        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PlaninaDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, PlaninaDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);



        //ukras na drvetu1
        glm::vec3 point_light_position_ukras1(-4.0f, 2.0f, 1.8f);
        CurrentShader->SetUniform3f("uKamenLight.Position", point_light_position_ukras1);
        CurrentShader->SetUniform1f("uKamenLight.Kc", 0.1 / abs(sin(StartTime )));
        CurrentShader->SetUniform1f("uKamenLight.Kl", 0.1 / abs(sin(StartTime )));
        CurrentShader->SetUniform1f("uKamenLight.Kq", 1.0 / abs(sin(StartTime )));
       /*
        
        glUseProgram(ColorShader.GetId());
        ColorShader.SetProjection(Projection);
        ColorShader.SetView(View);

          

        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, point_light_position_ukras1);
        model_matrix = glm::scale(model_matrix, glm::vec3(0.1));
        ColorShader.SetModel(model_matrix);

        unsigned int colorOffsetLocation = glGetUniformLocation(ColorShader.GetId(), "offset"); //Nadji adresu uniforme
        glUniform3f(colorOffsetLocation, 0.3, 0.5, 0.6);
        ColorShader.SetUniform4m("uMVP", Projection * View * model_matrix);
        */

        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, point_light_position_ukras1);
        model_matrix = glm::scale(model_matrix, glm::vec3(0.1));
        
        
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PlaninaDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, PlaninaDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);
        
        //ukras na drvetu2
        glm::vec3 point_light_position_ukras2( - 1.0f, 2.0f, 5.3f);
        CurrentShader->SetUniform3f("uKamenLight1.Position", point_light_position_ukras2);
        CurrentShader->SetUniform1f("uKamenLight1.Kc", 0.1 / abs(sin(StartTime)));
        CurrentShader->SetUniform1f("uKamenLight1.Kl", 0.1 / abs(sin(StartTime)));
        CurrentShader->SetUniform1f("uKamenLight1.Kq", 1.0 / abs(sin(StartTime)));
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, point_light_position_ukras2);
        model_matrix = glm::scale(model_matrix, glm::vec3(0.1));


        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PlaninaDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, PlaninaDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);


        //ukras na drvetu3
        glm::vec3 point_light_position_ukras3(-5.0f, 2.0f, 7.3f);
        CurrentShader->SetUniform3f("uKamenLight2.Position", point_light_position_ukras3);
        CurrentShader->SetUniform1f("uKamenLight2.Kc", 0.1 / abs(sin(StartTime)));
        CurrentShader->SetUniform1f("uKamenLight2.Kl", 0.1 / abs(sin(StartTime)));
        CurrentShader->SetUniform1f("uKamenLight2.Kq", 1.0 / abs(sin(StartTime)));
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, point_light_position_ukras3);
        model_matrix = glm::scale(model_matrix, glm::vec3(0.1));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PlaninaDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, PlaninaDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);
        /*
        //ukras na drvetu4
        glm::vec3 point_light_position_ukras3(-5.0f, 2.0f, 7.3f);
        CurrentShader->SetUniform3f("uKamenLight3.Position", point_light_position_ukras3);
        CurrentShader->SetUniform1f("uKamenLight2.Kc", 0.1 / abs(sin(StartTime)));
        CurrentShader->SetUniform1f("uKamenLight2.Kl", 0.1 / abs(sin(StartTime)));
        CurrentShader->SetUniform1f("uKamenLight2.Kq", 1.0 / abs(sin(StartTime)));
        model_matrix = glm::translate(model_matrix, glm::vec3(6.6f, 2.0f, 7.3f));
        model_matrix = glm::scale(model_matrix, glm::vec3(0.1));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PlaninaDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, PlaninaDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);
        
        //ukras na drvetu5
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(9.6f, 2.0f, 5.5f));
        model_matrix = glm::scale(model_matrix, glm::vec3(0.1));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PlaninaDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, PlaninaDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);
        */

        //ukras na drvetu6
        glm::vec3 point_light_position_ukras4(10.6f, 2.0f, 10.0f);
        CurrentShader->SetUniform3f("uKamenLight3.Position", point_light_position_ukras4);
        CurrentShader->SetUniform1f("uKamenLight3.Kc", 0.1 / abs(sin(StartTime)));
        CurrentShader->SetUniform1f("uKamenLight3.Kl", 0.1 / abs(sin(StartTime)));
        CurrentShader->SetUniform1f("uKamenLight3.Kq", 1.0 / abs(sin(StartTime)));
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(10.6f, 2.0f, 9.9f));
        model_matrix = glm::scale(model_matrix, glm::vec3(0.1));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PlaninaDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, PlaninaDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);

        //ukras na drvetu7
        glm::vec3 point_light_position_ukras5(5.6f, 2.0f, 11.0f);
        CurrentShader->SetUniform3f("uKamenLight4.Position", point_light_position_ukras5);
        CurrentShader->SetUniform1f("uKamenLight4.Kc", 0.1 / abs(sin(StartTime)));
        CurrentShader->SetUniform1f("uKamenLight4.Kl", 0.1 / abs(sin(StartTime)));
        CurrentShader->SetUniform1f("uKamenLight4.Kq", 1.0 / abs(sin(StartTime)));
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(5.6f, 2.0f, 10.9f));
        model_matrix = glm::scale(model_matrix, glm::vec3(0.1));
        CurrentShader->SetModel(model_matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PlaninaDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, PlaninaDiffuseTexture);
        glBindVertexArray(CubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, CubeVertices.size() / 8);
        glBindVertexArray(0);
        glUseProgram(0);
        glfwSwapBuffers(Window);

        // NOTE(Jovan): Time management
        EndTime = glfwGetTime();
        float WorkTime = EndTime - StartTime;
        if (WorkTime < TargetFrameTime) {
            int DeltaMS = (int)((TargetFrameTime - WorkTime) * 1000.0f);
            std::this_thread::sleep_for(std::chrono::milliseconds(DeltaMS));
            EndTime = glfwGetTime();
        }
        State.mDT = EndTime - StartTime;
    }

    glfwTerminate();
    return 0;
}
