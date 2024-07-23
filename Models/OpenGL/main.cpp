#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader_s.h"
#include "stb_image.h"
#include "camera.h"
#include "mesh.h"
#include "model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <random>

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera* camera;
Model* backpack;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float lastX = 400, lastY = 300;
float yaw = -90.0f, pitch = 0.0f;
bool firstMouse = true;

float fov = 60.0f;

float deltaTime = 0.0f;	// время между текущим и последним кадрами
float lastFrame = 0.0f; // время последнего кадра

const glm::vec3 StartLightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

float texturesProportion = 20.0f;
const float radius = 10.0f;

bool mode = true;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL application window", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Компилирование нашей шейдерной программы
    Shader modelShader("shader.vert", "model_loading.frag");

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(0.0f,  0.0f, -5.0f),
        glm::vec3(4.0f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -0.3f),
    };

    glm::vec3 pointLightColors[] = {
        glm::vec3(0.2f,  0.2f,  0.6f),
        glm::vec3(0.3f,  0.3f,  0.7f),
        glm::vec3(0.0f,  0.0f,  0.3f),
        glm::vec3(0.4f,  0.4f,  0.4f),
    };

    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    backpack = new Model("../source/Survival_BackPack/backpack.obj");

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Рендеринг
        glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Рендеринг ящиков
        modelShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        modelShader.setMat4("projection", projection);

        glm::mat4 view = camera->GetViewMatrix();;
        modelShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        modelShader.setMat4("model", model);

        backpack->Draw(modelShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete camera;
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && texturesProportion < 100.0f)
        texturesProportion += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && texturesProportion > 1.0f)
        texturesProportion -= 0.01f;

    // настройки камеры
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        mode = false;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
        mode = true;
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
    float yoffset = lastY - ypos; // уменьшаемое и вычитаемое поменяны местами, так как диапазон y-координаты определяется снизу вверх
    lastX = xpos;
    lastY = ypos;

    camera->ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera->ProcessMouseScroll(yoffset);
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}