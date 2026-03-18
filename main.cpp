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
            exit(EXIT_FAILURE);
        }

        // Установление стандартов GFLW
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        // Создание окна
        window = glfwCreateWindow(WIDTH, HEIGHT, "Black Hole Simulation", NULL, NULL);

        // Проверка создания окна
        if (!window && !error) {
            cerr << "Failed to create window" << endl;
            glfwTerminate();
            exit(EXIT_FAILURE);
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
            exit(EXIT_FAILURE);
        }

        // Создание переменной которая будет хранить адресс буфера
        GLuint vertexBuffer;
        // Создание буфера и записи его адреса
        glGenBuffers(1, &vertexBuffer);

        // Список с углами или как это говно называется, пофиг всё равно это временно
        float vertices[] = {
            0.0f, 0.5f, // Vertex 1 (X, Y)
            0.5f, -0.5f, // Vertex 2 (X, Y)
            -0.5f, -0.5f // Vertex 3 (X, Y)
        };

        //
        GLuint vbo;
        // Создание Vertex Buffer Object (VBO) и запись указателя на него а vbo
        glGenBuffers(1, &vbo);

        // Выбор и активация буфера по адресу vbo
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // Выделение памяти видеокрты и перенос вершин в неё
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // ШЕЙДЕРЫ

        // Запись шеёдеров в переменные

        // Вершинный (?) шейдер
        const char* vertexSource = R"glsl(
            #version 150 core
            in vec2 position;
            void main()
            {
                gl_Position = vec4(position, 0.0, 1.0);
            }
        )glsl";

        // Фрагментный (?) шейдер
        const char* fragmentSource = R"glsl(
            #version 150 core
            out vec4 outColor;
            void main()
            {
                outColor = vec4(1.0, 1.0, 1.0, 1.0);
            }
        )glsl";

        // Примерная структура того что происходит далее
        /*
        Создание переменной в которой хранится адресс    Создание шейдера как объекта (типо продукт класса)
        Указание на код шейдера
        Компиляция шейдера

        Проверка статуса шейдера и при ошибке вывод логов в переменную
        */

        // Вершинный шейдер
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexSource, NULL);
        glCompileShader(vertexShader);

        GLint statusv;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &statusv);
        if(!statusv){
            char bufferv[512];
            glGetShaderInfoLog(vertexShader, 512, NULL, bufferv);
            cerr << "Vertex shaider error" << endl << bufferv << endl;
        }

        // Фрагментный шейдер
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
        glCompileShader(fragmentShader);

        GLint statusf;
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &statusf);
        if(!statusf){
            char bufferf[512];
            glGetShaderInfoLog(vertexShader, 512, NULL, bufferf);
            cerr << "Fragment shaider error" << endl << bufferf << endl;
        }

        // Создание программы шейдеров и их к ней привязка
        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        // Выбор этой самой программы
        glUseProgram(shaderProgram);


        // Всё до этого момента я помню
        // А это что за дерьмо?

        //making the link between vertex data and attributes
        //retrieving a reference to the position input in the vertex shader
        GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
        //specifing how the data for that input is retrieved from the array
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
        //enabling the vertex attribute array
        glEnableVertexAttribArray(posAttrib);

    }
    // Проверка на получение ошибок
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

        if (glfwGetKey(engine.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(engine.window, GL_TRUE);
        }
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