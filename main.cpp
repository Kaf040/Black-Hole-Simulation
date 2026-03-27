#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cmath>
#include <vector>
#include <iostream>
using namespace glm;
using namespace std;

const double c = 299792458.0;
const double G = 6.67430e-11;
const double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233786783165271201909145648566923460348610454326648213393607260249141273724587006606315588174881520920962829254091715364367892590360011330530548820466521384146951941511609433057270365759591953092186117381932611793105118548074462;

const double pxtoco = (100000000000.0 * 2) / 800;

struct Ray;

// Список с лучами
vector<Ray> rays;

// Камера
struct Cam
{
    // Координаты
    double x, y;

    Cam(double X, double Y){
        x = X; y = Y;
    }
};

struct Mouse
{
    double x, y;
    double startx, starty;
    
    bool lkm, rkm;
    bool drawing;
    
    void run();
};

Mouse mouse;

struct Engine
{
    GLFWwindow* window;
    // Размеры окна в пикселях
    int WIDTH = 800;
    int HEIGHT = 600;
    // Размеры окна в метрах
    double width = 100000000000.0;
    double height = 75000000000.0;
    bool pause;

    Engine(){
        // Запуск glew
        glewExperimental = GL_TRUE;
        glewInit();

        // Проверка инициализации GFLW
        if (!glfwInit()) {
            cerr << "Failed to init GLFW" << endl;
            exit(EXIT_FAILURE);
        }

        // Создание окна
        window = glfwCreateWindow(WIDTH, HEIGHT, "Black Hole Simulation", NULL, NULL);

        // Проверка создания окна
        if (!window) {
            cerr << "Failed to create window" << endl;
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        // Установление минимального интервала между сменой буферов
        glfwSwapInterval(1);
        // Выбор созданного окна в качестве контекста
        glfwMakeContextCurrent(window);

        // Проверка работы GLEW
        if (glewInit() != GLEW_OK) {
            cerr << "Failed to init GLEW" << endl;
            glfwDestroyWindow(window);
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        pause = false;
        
    }
    
    void run(Cam cam) {
        // Отчищаем буфферы
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Отчищаем матрицы
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        // выставляем положение "камеры"
        double left   = -width + cam.x;
        double right  =  width + cam.x;
        double bottom = -height + cam.y;
        double top    =  height + cam.y;
        glOrtho(left, right, bottom, top, -1.0, 1.0);
        // Отчищаем матрицы
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
};

// Создание объекта движка
Engine engine;

void create_rays();

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
        if(!engine.pause){
            engine.pause = true;
        }
        else if(engine.pause){
            engine.pause = false;
        }
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS){
        while (!rays.empty())
        {
            rays.pop_back();
        }
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_F && action == GLFW_PRESS){
        create_rays();
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    double x, y;
    mouse.x = xpos - (engine.WIDTH / 2);
    mouse.y = (ypos - (engine.HEIGHT / 2)) * -1;
} 

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_RIGHT){
        if(action == GLFW_PRESS){
            mouse.rkm = true;
        }
        else if(action == GLFW_RELEASE){
            mouse.rkm = false;
        }
    }
    if(button == GLFW_MOUSE_BUTTON_LEFT){
        if(action == GLFW_PRESS){
            mouse.lkm = true;
        }
        else if(action == GLFW_RELEASE){
            mouse.lkm = false;
        }
    }
}

struct Hole{
    // Из ввода
    // Нормальные координаты
    double x, y;
    // Масса
    double m;

    // Из вычислений
    // Радиус Шварцшильда (горизонт событий)
    double r_s;

    // Присваивание значений и вычисление радиуса шварцшильда
    Hole(vec2 pos, double M)
    {
        x = pos.x; y = pos.y; m = M;
        r_s = 2.0 * G * M / (c * c);
    }
    
    void draw() {
        // Выбор режима последовательных треугольников
        glBegin(GL_TRIANGLE_STRIP);
        // Белый цвет
        glColor3f(1.0f, 1.0f, 1.0f);               
        for(int i = 0; i <= 100; i++) {
            // Находим нужный угол как i сотых из двух радиан
            double angle = 2.0f * PI * i / 100;
            double x = r_s * cos(angle);
            double y = r_s * sin(angle);
            // передача полученной точки в gl
            glVertex2f(x, y);
            
            // Находим угол как раньше но со смещением в пол шага
            float angle2 = (2.0f * PI * i / 100) + (2.0f * PI * 1.0f / 100);
            float x2 = (r_s - r_s * 0.1) * cos(angle);
            float y2 = (r_s - r_s * 0.1) * sin(angle);
            glVertex2f(x2, y2);
            
        }
        glEnd();
    } 

};

//Hole hole({0, 0}, 8.54e36);
Hole hole({0, 0}, 8.54e36);

struct Ray{
    // нормальные координаты
    double x, y;

    double dx, dy;

    // полярные координаты
    // широта, долгота, расстояние до центра
    double phi, r;

    //Производные радиальных координат (скорость их изменения)
    double dphi, dr;

    double d2phi = 0, d2r = 0;
    
    // Список с точками для следа
    vector<vec2> trail;

    // 
    Ray(vec2 pos, vec2 vel) {
        x = pos.x; y = pos.y;
        r = sqrt(x * x + y * y);
        phi = atan2(y, x);

        dr = vel.x * cos(phi) + vel.y * sin(phi);
        dphi  = ( -vel.x * sin(phi) + vel.y * cos(phi) ) / r;

        double f = 1.0 - hole.r_s / r;  
    }

    //
    void step(double r_s, double dλ) {
        /*
        r = sqrt(x * x + y * y);
        phi = atan(y / x);
        theta = atan(z / x);
        */

        //
        if(r_s >= r){
            trail.push_back({x, y});
            return;
        }

        

        dr = dr + (d2r * dλ);
        dphi = dphi + (d2phi * dλ);
        r = r + (dr * dλ);
        phi = phi + (dphi * dλ);


        //
        x = hole.x + cos(phi) * r;
        y = hole.y + sin(phi) * r;

        //
        trail.push_back({x, y});
    }

    void draw(Ray& ray) {
        glPointSize(2.0f);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_POINTS);
            glVertex2f(ray.x, ray.y);
        glEnd();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glLineWidth(2.0f);
               
            int n = ray.trail.size();
            int s;
            if(n > 3000){
                s = n - 3000;
            }
            else {
                s = 0;
            }
            
            glBegin(GL_LINE_STRIP);
            for(int i = s; i < n; i++){
                float alpha = float(i - s) / float(n - s - 1);
                glColor4f(1.0f, 1.0f, 1.0f, alpha);
                glVertex2f(ray.trail[i].x, ray.trail[i].y);
            }
            glEnd();
        
    }
};

void Mouse::run(){
    if(lkm && !drawing){
        startx = x;
        starty = y;
        drawing = true;
    }
    else if(!lkm && drawing){
        drawing = false;
        //creation of rays
        double rx, ry, a, cx, cy, len, otst;
        a = atan2(starty - y, startx - x);
        a += PI / 2;
        if(a - atan(y, x) < PI / 2 && a - atan(y, x) > (PI / 2) * -1){
            a -= PI;
        }
        cx = cos(a) * c;
        cy = sin(a) * c;
        
        a = atan2(y - starty, x - startx);
        rx = startx;
        ry = starty;
        len = sqrt((x - startx) * (x - startx) + (y - starty) * (y - starty));
        otst = len;
        while (otst > 4500000000 / pxtoco)
        {
            otst -= 4500000000 / pxtoco;
        }
        otst = otst / 2;

        while (otst < len)
        {
        rx = (cos(a) * otst) + startx;
        ry = (sin(a) * otst) + starty;
        rays.push_back(Ray({rx * pxtoco, ry * pxtoco}, {cx, cy}));
        otst += 4500000000 / pxtoco;
        }
    }
    if(drawing){
        glBegin(GL_LINES);
        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(400.0f);
        glVertex2f(startx * pxtoco, starty * pxtoco);
        glVertex2f(x * pxtoco, y * pxtoco);
        glEnd();
    }
}

Cam cam(0, 0);

void geodesic(Ray& ray, double r_s){
    double r = ray.r;       double dr = ray.dr;       double d2r = ray.d2r;
    double phi = ray.phi;   double dphi = ray.dphi;   double d2phi = ray.d2phi;

    d2r = (r_s / (2.0 * r * (r - r_s))) * (dr * dr) + (r - r_s) * (dphi * dphi);
    d2phi = -2.0 * dr * dphi / r;

    ray.d2r = d2r;
    ray.d2phi = d2phi;
}

void create_rays(){
    for(int i = 0; i < 30; i++){
        double y = ((2 * engine.height * i / 30) - engine.height) * 0.9;
        rays.push_back(Ray({-75000000000.0, y}, {c, 0}));
    }
}

int main() {

            
    glfwSetKeyCallback(engine.window, key_callback);
    glfwSetCursorPosCallback(engine.window, cursor_position_callback);
    glfwSetMouseButtonCallback(engine.window, mouse_button_callback);

    // Главный цикл
    while (!glfwWindowShouldClose(engine.window)) {

        engine.run(cam);
        mouse.run();
        

        for(auto& ray : rays){
            if(!engine.pause){
                for(int i = 0; i < 10; i++){
                geodesic(ray, hole.r_s);
                ray.step(hole.r_s, 0.1);
                }
            }
            ray.draw(ray);
        }

        hole.draw();

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