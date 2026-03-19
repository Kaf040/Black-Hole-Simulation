#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
using namespace glm;
using namespace std;

const double c = 299792458.0;
const double G = 6.67430e-11;
const double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233786783165271201909145648566923460348610454326648213393607260249141273724587006606315588174881520920962829254091715364367892590360011330530548820466521384146951941511609433057270365759591953092186117381932611793105118548074462;

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
    // Проверка на получение ошибок
    bool check() {
        if(error)
        {return true;}
        else{return false;}
    }
};

struct ray{
    // global coordinates
    double x, y, z;
    // radial coordinates 
    double teta, phi, r;
    
};

struct Hole{
    // Из ввода
    // Нормальные координаты
    double x, y, z;
    // Масса
    double m;
    
    // Из вычислений
    // Радиус Шварцшильда (горизонт событий)
    double r_s;
    
    Hole(double X, double Y, double Z, double mass)
    {
        x = X; y = Y; z = Z; M = mass;
        r_s = 2.0 * G * mass / (c * c);
    }
    void draw() {
        glBegin(GL_TRIANGLE_STRIP);
        glColor3f(1.0f, 1.0f, 3.0f);               // white color for the black hole
        for(int i = 0; i <= 100; i++) {
            float angle = 2.0f * PI * i / 100;
            float x = r_s * cos(angle); // Radius of 0.1
            float y = r_s * sin(angle);
            glVertex2f(x, y);
            
            float angle2 = (2.0f * PI * i / 100) + (2.0f * PI * 1.0f / 100);
            float x2 = (r_s - r_s * 0.1) * cos(angle); // Radius of 0.1
            float y2 = (r_s - r_s * 0.1) * sin(angle);
            glVertex2f(x, y);
        }
        glEnd();
    } 
    
};

vector<ray> rays;

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