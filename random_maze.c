#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define W 40
#define H 19
void gen(char m[H][W]) {
    for(int y=0;y<H;y++) for(int x=0;x<W;x++) m[y][x]='#';
    for(int y=1;y<H-1;y+=2) {
        for(int x=1;x<W-1;x+=2) {
            m[y][x]=' ';
            if(y>1&&(rand()%2)) m[y-1][x]=' ';
            else if(x>1) m[y][x-1]=' ';
        }
    }
    m[1][0]=' '; m[H-2][W-1]=' ';
}
int main() {
    srand(time(NULL));
    char m[H][W];
    gen(m);
    for(int y=0;y<H;y++) {
        for(int x=0;x<W;x++) putchar(m[y][x]);
        putchar('\n');
    }
    return 0;
}
