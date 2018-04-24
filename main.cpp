#include "Model.h"
#include <GLFW/glfw3.h>

void key_cb(GLFWwindow *window, int key, int scancode, int action, int mode);

int main() {
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

    glEnable(GL_DEPTH_TEST);

    Program modelProg(R"(Shaders\vertex.glsl)", R"(Shaders\fragment.glsl)");
    Program lightProg(R"(Shaders\lightVert.glsl)", R"(Shaders\lightFrag.glsl)");

    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glm::mat4 model(1.0f), view(1.0f), projection(1.0f), model1;
    projection = glm::perspective(glm::radians(45.0f),
                                  (GLfloat)window_w/(GLfloat)window_h, 0.001f, 100.0f);
    glm::vec3 viewPos = glm::vec3(0.0f, 0.0f, -3.0f);
    view = glm::translate(view, viewPos);

    /*Model myModel("C:\\Users\\Murchan\\Desktop\\cat.obj");
    myModel.mvp_matrix = MVP_matrix(model, view, projection);*/

    Model myModel1("C:\\Users\\Murchan\\Desktop\\cube.obj");
    myModel1.mvp_matrix = MVP_matrix(model1, view, projection);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(.2f, .3f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /*myModel.mvp_matrix.model = glm::mat4(1.0f);
        myModel.mvp_matrix.model = glm::rotate(myModel.mvp_matrix.model,
                                       (GLfloat)glfwGetTime() * glm::radians(20.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f));
        myModel.mvp_matrix.model = glm::translate(myModel.mvp_matrix.model, glm::vec3(-0.5f, 0.0f, 0.0f));*/

        myModel1.mvp_matrix.model = glm::mat4(1.0f);
        myModel1.mvp_matrix.model = glm::translate(myModel1.mvp_matrix.model,
                                                   glm::vec3(0.2f, 0.0f, 0.0f));
        myModel1.mvp_matrix.model = glm::rotate(myModel1.mvp_matrix.model,
                                       (GLfloat)glfwGetTime() * glm::radians(-20.0f),
                                       glm::vec3(0.0f, 1.0f, 0.0f));
        //myModel1.mvp_matrix.model = glm::scale(myModel1.mvp_matrix.model, glm::vec3(0.2f));

        //myModel.draw(modelProg, myModel.mvp_matrix);
        myModel1.draw(modelProg, myModel1.mvp_matrix);

        glfwSwapBuffers(window);
    }
    glfwTerminate();
    std::cout << "glfwTerminate() success" << std::endl;
    return 0;
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
