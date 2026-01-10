//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright  2016 CGIS. All rights reserved.
//

#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"
#include "Objects.hpp"

#include <iostream>
#include <vector>

// Camera variables
bool godMode = false;
float cameraSpeed = 0.01f;
const float maxCameraSpeed = 0.05f;
const float minCameraSpeed = 0.01f;
const float cameraSpeedIncrement = 0.005f;

int glWindowWidth = 800;
int glWindowHeight = 600;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 2048 * 4;
const unsigned int SHADOW_HEIGHT = 2048 * 4;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

GLuint lightSpaceTrMatrixLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

glm::vec3 pointLightPos = glm::vec3(5.0f, 2.0f, 0.0f);
glm::vec3 pointLightColor = glm::vec3(0.0f, 0.2f, 0.4f);
bool pointLightOn = true;

bool flashlightOn = false;
float fogDensity = 0.05f;

gps::Camera myCamera(
    glm::vec3(0.0f, 0.0f, 5.5f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

bool pressedKeys[1024];
float angleY = 180.0f;
GLfloat lightAngle;


gps::Scene myScene;

gps::SkyBox skyBox;
gps::Shader skyboxShader;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

const GLfloat nearPlane = 0.01f;
const GLfloat farPlane = 50.0f;
const float orthoConst = 80.0f;

// ANIMATION VARIABLES
bool isAnimating = false;
float animationTime = 0.0f;
std::vector<glm::vec3> cameraPath = {
    glm::vec3(0.0f, 2.5f, 15.0f),
    glm::vec3(12.0f, 2.0f, 12.0f),
    glm::vec3(15.0f, 2.5f, 0.0f),
    glm::vec3(10.0f, 2.0f, -10.0f),
    glm::vec3(0.0f, 3.0f, -15.0f),
    glm::vec3(-10.0f, 2.0f, -10.0f),
    glm::vec3(-15.0f, 2.5f, 0.0f),
    glm::vec3(-12.0f, 2.0f, 12.0f)
};

GLenum glCheckError_(const char* file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (key == GLFW_KEY_5 && action == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

    // Toggle Animation
    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
        isAnimating = !isAnimating;
        animationTime = 0.0f;
    }

    //Toggle flashlight

    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        flashlightOn = !flashlightOn;
    }
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            pressedKeys[key] = true;
        else if (action == GLFW_RELEASE)
            pressedKeys[key] = false;
    }


}

bool firstMouse = true;
double lastX = 400.0;
double lastY = 300.0;
float yaw = -90.0f;
float pitch = 0.0f;

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (isAnimating) return;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos - lastX);
    float yoffset = static_cast<float>(lastY - ypos);
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05f;
    yoffset *= sensitivity;

    yaw += xoffset * sensitivity;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    myCamera.rotate(pitch, yaw);
}

float lastPress = 0.0f;
float debounceThreshold = 0.2f;

float decelerationThreshold = 0.4f;
float lastDeceleration = 0.0f;

void processMovement()
{
    if (pressedKeys[GLFW_KEY_Q]) {
        angleY -= 1.0f;
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angleY += 1.0f;
    }

    if (pressedKeys[GLFW_KEY_J]) {
        lightAngle -= 1.0f;
    }

    if (pressedKeys[GLFW_KEY_L]) {
        lightAngle += 1.0f;
    }

    // Disable manual camera movement if animating
    if (!isAnimating) {
        glm::vec3 originalPosition = myCamera.getPosition(); 


        auto attemptMove = [&](gps::MOVE_DIRECTION dir) {
            myCamera.move(dir, cameraSpeed, godMode);
            if (!godMode && myScene.checkCollision(myCamera.getPosition())) {
                myCamera.setPosition(originalPosition);
            }
            };

        if (pressedKeys[GLFW_KEY_W]) attemptMove(gps::MOVE_FORWARD);
        if (pressedKeys[GLFW_KEY_S]) attemptMove(gps::MOVE_BACKWARD);
        if (pressedKeys[GLFW_KEY_A]) attemptMove(gps::MOVE_LEFT);
        if (pressedKeys[GLFW_KEY_D]) attemptMove(gps::MOVE_RIGHT);
    }

    if (pressedKeys[GLFW_KEY_LEFT_SHIFT])
    {
        if (glfwGetTime() - lastPress > debounceThreshold)
        {
            godMode = !godMode;
            std::cout << "CHANGED GODMODE!" << std::endl;
            lastPress = glfwGetTime();
        }
    }

    if (pressedKeys[GLFW_KEY_6])
    {
        if (glfwGetTime() - lastPress > debounceThreshold)
        {
            pointLightOn = !pointLightOn;
            lastPress = glfwGetTime();
        }
    }

    if (pressedKeys[GLFW_KEY_7])
    {
        if (glfwGetTime() - lastPress > debounceThreshold)
        {
            fogDensity += 0.005f;
            lastPress = glfwGetTime();
        }
    }

    if (pressedKeys[GLFW_KEY_8])
    {
        if (glfwGetTime() - lastPress > debounceThreshold)
        {
            if (fogDensity > 0.0f) fogDensity -= 0.005f;
            lastPress = glfwGetTime();
        }
    }

    if (pressedKeys[GLFW_KEY_1]) {
        if (glfwGetTime() - lastPress > debounceThreshold)
        {
            if (cameraSpeed > minCameraSpeed) {
                cameraSpeed -= cameraSpeedIncrement;
            }
            lastPress = glfwGetTime();
        }
    }
    if (pressedKeys[GLFW_KEY_LEFT_CONTROL]) {
        if (glfwGetTime() - lastPress > debounceThreshold)
        {
            if (cameraSpeed < maxCameraSpeed) {
                cameraSpeed += cameraSpeedIncrement;
            }
            lastPress = glfwGetTime();
        }
    }
    else if (!pressedKeys[GLFW_KEY_LEFT_CONTROL]) {
        if (glfwGetTime() - lastDeceleration > decelerationThreshold)
        {
            if (cameraSpeed > minCameraSpeed) {
                cameraSpeed -= cameraSpeedIncrement;
            }
            lastDeceleration = glfwGetTime();
        }
    }
}

// https://en.wikipedia.org/wiki/Catmull%E2%80%93Rom_spline
// Catmull-Rom Spline calculation
glm::vec3 getSplinePoint(float t, std::vector<glm::vec3> points) {
    int p0, p1, p2, p3;
    int n = points.size();

    p1 = (int)t % n;
    p2 = (p1 + 1) % n;
    p3 = (p2 + 1) % n;
    p0 = p1 >= 1 ? p1 - 1 : n - 1;

    t = t - (int)t;

    float tt = t * t;
    float ttt = tt * t;

    float q1 = -ttt + 2.0f * tt - t;
    float q2 = 3.0f * ttt - 5.0f * tt + 2.0f;
    float q3 = -3.0f * ttt + 4.0f * tt + t;
    float q4 = ttt - tt;

    return 0.5f * (points[p0] * q1 + points[p1] * q2 + points[p2] * q3 + points[p3] * q4);
}

bool initOpenGLWindow()
{
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //window scaling for HiDPI displays
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    //for sRBG framebuffer
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

    //for antialising
    glfwWindowHint(GLFW_SAMPLES, 4);

    glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
    if (!glWindow) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        return false;
    }

    glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
    glfwSetKeyCallback(glWindow, keyboardCallback);
    glfwSetCursorPosCallback(glWindow, mouseCallback);
    glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(glWindow);

    glfwSwapInterval(1);

#if not defined (__APPLE__)
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
#endif

    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    //for RETINA display
    glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

    return true;
}

void initOpenGLState()
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glViewport(0, 0, retina_width, retina_height);

    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void initObjects() {
    // Delegated to Scene class
    myScene.loadModels();
}

void initShaders() {
    myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
    myCustomShader.useShaderProgram();
    lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
    lightShader.useShaderProgram();
    depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
    depthMapShader.useShaderProgram();

    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
}

void initUniforms() {
    myCustomShader.useShaderProgram();

    model = glm::mat4(1.0f);
    modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
    projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

    //set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "pointLightColor"), 1, glm::value_ptr(pointLightColor));

    lightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    depthMapShader.useShaderProgram();
    glUniform1i(glGetUniformLocation(depthMapShader.shaderProgram, "diffuseTexture"), 0);
}

void initSkybox() {
    std::vector<const GLchar*> faces;
    faces.push_back("skybox/right.png");
    faces.push_back("skybox/left.png");
    faces.push_back("skybox/top.png");
    faces.push_back("skybox/bottom.png");
    faces.push_back("skybox/back.png");
    faces.push_back("skybox/front.png");

    skyBox.Load(faces);
}

void initFBO() {
    glGenFramebuffers(1, &shadowMapFBO);

    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


glm::mat4 computeLightSpaceTrMatrix() {
    glm::vec3 worldLightDir = glm::vec3(lightRotation * glm::vec4(lightDir, 0.0f));
    glm::mat4 lightView = glm::lookAt(worldLightDir * 20.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProjection = glm::ortho(-orthoConst, orthoConst, -orthoConst, orthoConst, nearPlane, farPlane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
    return lightSpaceTrMatrix;
}
void renderScene() {

    // depth maps creation pass (Internal shadow generation)
    depthMapShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    glCullFace(GL_FRONT);
    // Draw via Scene class
    myScene.draw(depthMapShader, view, angleY);
    glCullFace(GL_BACK);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // final scene rendering pass (with shadows)
    glViewport(0, 0, retina_width, retina_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myCustomShader.useShaderProgram();

    // Update Camera for Aniation 
    if (isAnimating) {
        animationTime += 0.005f; // Animation speed
        glm::vec3 animPos = getSplinePoint(animationTime, cameraPath);
        view = glm::lookAt(animPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else {
        view = myCamera.getViewMatrix();
    }

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // Update Flashlight (Spotlight)
    glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "flashlightOn"), flashlightOn ? 1 : 0);

    if (flashlightOn) {
        // Set flashlight direction in eye space (forward)
        glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLightDir"), 0.0f, 0.0f, -1.0f);

        // Set cone angles for soft edges
        glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLightCutOff"), glm::cos(glm::radians(12.5f)));
        glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "spotLightOuterCutOff"), glm::cos(glm::radians(17.5f)));
    }

    // Set the light direction (direction towards the light)
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

    glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "pointLightOn"), pointLightOn ? 1 : 0);
    glm::vec3 pLightEye = glm::vec3(view * glm::vec4(pointLightPos, 1.0f));
    glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "pointLightPos"), 1, glm::value_ptr(pLightEye));
    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "fogDensity"), fogDensity);

    // bind the shadow map
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

    glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));

    // Draw via Scene class
    myScene.draw(myCustomShader, view, angleY);

    // Draw Light Cube via Scene class
    // Only draw the sun cube if flashlight is off
    if (!flashlightOn) {
        myScene.drawLightCube(lightShader, view, lightRotation, lightDir);
    }

    // Draw skybox last
    skyboxShader.useShaderProgram();
    glUniform1f(glGetUniformLocation(skyboxShader.shaderProgram, "fogDensity"), fogDensity);
    skyBox.Draw(skyboxShader, view, projection);
}
void cleanup() {
    glDeleteTextures(1, &depthMapTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &shadowMapFBO);
    glfwDestroyWindow(glWindow);
    glfwTerminate();
}

int main(int argc, const char* argv[]) {

    if (!initOpenGLWindow()) {
        glfwTerminate();
        return 1;
    }

    initOpenGLState();
    initObjects();
    initShaders();
    initUniforms();
    initFBO();
    initSkybox();

    glCheckError();

    while (!glfwWindowShouldClose(glWindow)) {
        processMovement();
        renderScene();

        glfwPollEvents();
        glfwSwapBuffers(glWindow);
    }

    cleanup();

    return 0;
}