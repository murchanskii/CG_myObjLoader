#include "Model.h"
#include <GLFW/glfw3.h>

void key_cb(GLFWwindow *window, int key, int scancode, int action, int mode);
void cursor_cb(GLFWwindow *window, double xpos, double ypos);
void scroll_cb(GLFWwindow *window, double xoffset, double yoffset);
void mouse_button_cb(GLFWwindow* window, int button, int action, int mods);
void get_model_path(std::string &path_to_model, const std::string &path);

GLfloat lastX, lastY, yaw, pitch, kscale;
bool mouse_pressed;

int main() {
    mouse_pressed = false;
    kscale = 1.0f;
    yaw = 0.0f;
    pitch = 0.0f;
    if (!glfwInit()) {
        std::cout << "Error glwfInit()" << std::endl;
        exit(EXIT_FAILURE);
    }

    GLuint window_w = 1000, window_h = 1000;

    std::cout << "glfwInit() success" << std::endl;
    std::string window_title = "Hello Window";
    GLFWwindow* window = glfwCreateWindow(window_w, window_h,
                                          window_title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSetKeyCallback(window, key_cb);
    glfwSetCursorPosCallback(window, cursor_cb);
    glfwSetCursorPos(window, lastX, lastY);
    glfwSetMouseButtonCallback(window, mouse_button_cb);
    glfwSetScrollCallback(window, scroll_cb);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Program modelProg(R"(Shaders\vertex.glsl)", R"(Shaders\fragment.glsl)");

    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glm::mat4 model(1.0f), view(1.0f), projection(1.0f);
    projection = glm::perspective(glm::radians(45.0f),
                                  (GLfloat)window_w/(GLfloat)window_h, 0.001f, 100.0f);
    glm::vec3 viewPos = glm::vec3(0.0f, 0.0f, -3.0f);
    view = glm::translate(view, viewPos);

    std::string path_to_model;
    get_model_path(path_to_model, "models.ini");
    Model myModel(path_to_model);
    myModel.mvp_matrix = MVP_matrix(model, view, projection);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(.3f, .3f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myModel.mvp_matrix.model = glm::mat4(1.0f);
        myModel.mvp_matrix.model = glm::rotate(myModel.mvp_matrix.model,
                                       glm::radians(yaw),
                                       glm::vec3(0.0f, 1.0f, 0.0f));
        myModel.mvp_matrix.model = glm::rotate(myModel.mvp_matrix.model,
                                               glm::radians(pitch),
                                               glm::vec3(1.0f, 0.0f, 0.0f));
        myModel.mvp_matrix.model = glm::scale(myModel.mvp_matrix.model,
                                              glm::vec3(kscale));

        myModel.draw(modelProg, myModel.mvp_matrix);

        glfwSwapBuffers(window);
    }
    glfwTerminate();
    std::cout << "glfwTerminate() success" << std::endl;
    return 0;
}

void get_model_path(std::string &path_to_model, const std::string &path) {
    file_exists(path);
    std::ifstream file;
    bool no_file = true;

    file.open(path);
    while (std::getline(file, path_to_model)) {
        if (path_to_model.substr(0, 1) != "#") {
            no_file = false;
            break;
        }
    }
    file.close();
    if (no_file) {
        std::cout << "ERROR:: No file" << std::endl;
        exit(1);
    }
}

void key_cb(GLFWwindow *window, int key, int scancode, int action, int mode) {
    static bool keys[1024];
    if (action == GLFW_PRESS)
        keys[key] = true;
    else if (action == GLFW_RELEASE)
        keys[key] = false;

    if (keys[key]) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
            case GLFW_KEY_ENTER:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            default:
                break;
        }
    }
}

void cursor_cb(GLFWwindow *window, double xpos, double ypos) {
    if (mouse_pressed) {
        GLfloat xoffset = xpos - lastX;
        GLfloat yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        GLfloat sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;
    }
    else {
        lastX = xpos;
        lastY = ypos;
    }
}

void mouse_button_cb(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mouse_pressed = true;
        glfwSetCursorPos(window, lastX, lastY);
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        mouse_pressed = false;
        glfwSetCursorPos(window, lastX, lastY);
    }
}

void scroll_cb(GLFWwindow *window, double xoffset, double yoffset) {
    float sensitivity = 0.01f;
    if (yoffset < 0.0f) {
        if (kscale > 0.1)
            kscale -= sensitivity;
    }
    if (yoffset > 0.0f) {
        kscale += sensitivity;
    }
}