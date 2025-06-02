#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

// Настройки программы
#define WIDTH 40              // Ширина экрана
#define HEIGHT 20             // Высота экрана
#define PI 3.14159265358979323846
#define DEFAULT_DISTANCE 100.0  // Дистанция камеры
#define ROTATION_SPEED 0.03   // Базовая скорость вращения
#define FRAME_DELAY 30000     // Задержка между кадрами (в микросекундах)
#define DRAW_CHAR '#'         // Символ для отрисовки куба

volatile sig_atomic_t keep_running = 1;

void handle_signal(int sig) {
    keep_running = 0;
}

void clear_screen() {
    printf("\033[2J\033[H");
}

typedef struct { float x, y, z; } Point3D;
typedef struct { int x, y; } Point2D;

// Вершины куба (центрированы в начале координат)
Point3D cube[] = {
    {-1,-1,-1}, {1,-1,-1}, {1,1,-1}, {-1,1,-1},
    {-1,-1,1}, {1,-1,1}, {1,1,1}, {-1,1,1}
};

// Ребра куба (соединения между вершинами)
int edges[12][2] = {
    {0,1}, {1,2}, {2,3}, {3,0},  // Нижняя грань
    {4,5}, {5,6}, {6,7}, {7,4},  // Верхняя грань
    {0,4}, {1,5}, {2,6}, {3,7}   // Боковые ребра
};

Point2D project(Point3D p, float angleX, float angleY, float angleZ, float distance) {
    // Поворот вокруг оси X
    float cosX = cos(angleX), sinX = sin(angleX);
    float y = p.y * cosX - p.z * sinX;
    float z = p.y * sinX + p.z * cosX;
    p.y = y; p.z = z;
    
    // Поворот вокруг оси Y (исправлено для правильной ориентации)
    float cosY = cos(angleY), sinY = sin(angleY);
    float x = p.x * cosY + p.z * sinY;
    z = -p.x * sinY + p.z * cosY;
    p.x = x; p.z = z;
    
    // Поворот вокруг оси Z
    float cosZ = cos(angleZ), sinZ = sin(angleZ);
    x = p.x * cosZ - p.y * sinZ;
    y = p.x * sinZ + p.y * cosZ;
    p.x = x; p.y = y;
    
    // Перспективная проекция с центрированием
    float factor = distance / (distance + p.z);
    return (Point2D){
        (int)(WIDTH/2 + p.x * factor * WIDTH/5),   // Уменьшен масштаб (WIDTH/5 вместо WIDTH/4)
        (int)(HEIGHT/2 - p.y * factor * HEIGHT/5)  // Центрирование по вертикали
    };
}

void draw_line(Point2D p1, Point2D p2, char buffer[HEIGHT][WIDTH]) {
    int dx = abs(p2.x - p1.x), sx = p1.x < p2.x ? 1 : -1;
    int dy = -abs(p2.y - p1.y), sy = p1.y < p2.y ? 1 : -1; 
    int err = dx + dy, e2;
    
    while(1) {
        if(p1.x >= 0 && p1.x < WIDTH && p1.y >= 0 && p1.y < HEIGHT) {
            buffer[p1.y][p1.x] = DRAW_CHAR;
        }
        if(p1.x == p2.x && p1.y == p2.y) break;
        e2 = 2*err;
        if(e2 >= dy) { err += dy; p1.x += sx; }
        if(e2 <= dx) { err += dx; p1.y += sy; }
    }
}

int main() {
    signal(SIGINT, handle_signal);
    
    float angleX = 0, angleY = 0, angleZ = 0;
    float distance = DEFAULT_DISTANCE;
    
    printf("Управление:\n");
    printf("  - Нажмите Ctrl+C для выхода\n");
    printf("  - Размеры экрана: %dx%d\n", WIDTH, HEIGHT);
    printf("  - Используемый символ: '%c'\n", DRAW_CHAR);
    printf("\nЗагрузка 3D куба...\n");
    usleep(1000000); // Пауза 1 секунда перед началом
    
    while(keep_running) {
        clear_screen();
        char buffer[HEIGHT][WIDTH];
        
        // Очистка буфера
        for(int y = 0; y < HEIGHT; y++)
            for(int x = 0; x < WIDTH; x++)
                buffer[y][x] = ' ';
        
        // Отрисовка ребер куба
        for(int i = 0; i < 12; i++) {
            Point2D p1 = project(cube[edges[i][0]], angleX, angleY, angleZ, distance);
            Point2D p2 = project(cube[edges[i][1]], angleX, angleY, angleZ, distance);
            draw_line(p1, p2, buffer);
        }
        
        // Вывод буфера на экран
        for(int y = 0; y < HEIGHT; y++) {
            for(int x = 0; x < WIDTH; x++) {
                putchar(buffer[y][x]);
            }
            putchar('\n');
        }
        
        // Обновление углов вращения
        angleX += ROTATION_SPEED * 0.7;
        angleY += ROTATION_SPEED;
        angleZ += ROTATION_SPEED * 0.3;
        
        usleep(FRAME_DELAY);
    }
    
    clear_screen();
    printf("Программа завершена.\n");
    return 0;
}
