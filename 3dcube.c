//3d cube:D by elliktronic
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>


#define WIDTH 40             
#define HEIGHT 20             
#define PI 3.14159265358979323846
#define DEFAULT_DISTANCE 100.0  
#define ROTATION_SPEED 0.03   
#define FRAME_DELAY 30000    
#define DRAW_CHAR '#'        

volatile sig_atomic_t keep_running = 1;

void handle_signal(int sig) {
    keep_running = 0;
}

void clear_screen() {
    printf("\033[2J\033[H");
}

typedef struct { float x, y, z; } Point3D;
typedef struct { int x, y; } Point2D;


Point3D cube[] = {
    {-1,-1,-1}, {1,-1,-1}, {1,1,-1}, {-1,1,-1},
    {-1,-1,1}, {1,-1,1}, {1,1,1}, {-1,1,1}
};


int edges[12][2] = {
    {0,1}, {1,2}, {2,3}, {3,0}, 
    {4,5}, {5,6}, {6,7}, {7,4},  
    {0,4}, {1,5}, {2,6}, {3,7}   
};

Point2D project(Point3D p, float angleX, float angleY, float angleZ, float distance) {

    float cosX = cos(angleX), sinX = sin(angleX);
    float y = p.y * cosX - p.z * sinX;
    float z = p.y * sinX + p.z * cosX;
    p.y = y; p.z = z;
    
  
    float cosY = cos(angleY), sinY = sin(angleY);
    float x = p.x * cosY + p.z * sinY;
    z = -p.x * sinY + p.z * cosY;
    p.x = x; p.z = z;
    

    float cosZ = cos(angleZ), sinZ = sin(angleZ);
    x = p.x * cosZ - p.y * sinZ;
    y = p.x * sinZ + p.y * cosZ;
    p.x = x; p.y = y;
    

    float factor = distance / (distance + p.z);
    return (Point2D){
        (int)(WIDTH/2 + p.x * factor * WIDTH/5),  
        (int)(HEIGHT/2 - p.y * factor * HEIGHT/5) 
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
    usleep(1000000); 
    
    while(keep_running) {
        clear_screen();
        char buffer[HEIGHT][WIDTH];
        

        for(int y = 0; y < HEIGHT; y++)
            for(int x = 0; x < WIDTH; x++)
                buffer[y][x] = ' ';
        
       
        for(int i = 0; i < 12; i++) {
            Point2D p1 = project(cube[edges[i][0]], angleX, angleY, angleZ, distance);
            Point2D p2 = project(cube[edges[i][1]], angleX, angleY, angleZ, distance);
            draw_line(p1, p2, buffer);
        }

        for(int y = 0; y < HEIGHT; y++) {
            for(int x = 0; x < WIDTH; x++) {
                putchar(buffer[y][x]);
            }
            putchar('\n');
        }
        

        angleX += ROTATION_SPEED * 0.7;
        angleY += ROTATION_SPEED;
        angleZ += ROTATION_SPEED * 0.3;
        
        usleep(FRAME_DELAY);
    }
    
    clear_screen();
    printf("Программа завершена.\n");
    return 0;
}
