#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
using namespace glm;
using namespace std;

double c = 299792458.0;
double G = 6.67430e-11;

struct Engine
{
    GLFWwindow* window;
    // Размеры окна в пикселях
    int WIDTH = 800;
    int HEIGHT = 600;
    // Размеры окна в метрах
    double width = 100000000000.0;
    double height = 75000000000.0;
    bool error = false;

    Engine(){
        // Запуск glew
        glewExperimental = GL_TRUE;
        glewInit();

        // Проверка инициализации GFLW
        if (!glfwInit()) {
            cerr << "Failed to init GLFW" << endl;
            error = true;
        }

        // Создание окна
        window = glfwCreateWindow(WIDTH, HEIGHT, "Black Hole Simulation", NULL, NULL);

        // Проверка создания окна
        if (!window && !error) {
            cerr << "Failed to create window" << endl;
            glfwTerminate();
            error = true;
        }

        // Установление минимального интервала между сменой буферов
        glfwSwapInterval(1);
        // Выбор созданного окна в качестве контекста
        glfwMakeContextCurrent(window);

        // Проверка работы GLEW
        if (glewInit() != GLEW_OK && !error) {
            cerr << "Failed to init GLEW" << endl;
            glfwDestroyWindow(window);
            glfwTerminate();
            error = true;
        }
    }
    // ПРоверка на получение ошибок
    bool check() {
        if(error)
        {return true;}
        else{return false;}
    }
};

Engine engine;

int main() {

    // Главный цикл
    while (!glfwWindowShouldClose(engine.window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Смена буффера
        glfwSwapBuffers(engine.window);

        // Запрос действий
        glfwPollEvents();
    }

    // Уничтожение окна
    glfwDestroyWindow(engine.window);
    // Уничтожение GFLW
    glfwTerminate();

    return 0;
}