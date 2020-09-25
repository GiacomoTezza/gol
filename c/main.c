#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (x), (y))
#define cursorup(x) printf("\033[%dA", (x))
#define cursordown(x) printf("\033[%dB", (x))
#define cursorforward(x) printf("\033[%dC", (x))
#define cursorbackward(x) printf("\033[%dD", (x))
#define backgroundblack() printf("\033[32;40m")
#define backgroundred() printf("\033[32;41m")
#define backgroundgreen() printf("\033[32;42m")


void update(int *board, int *old_gen, int rows, int cols) {
    int i = 0;
    int j = 0;
    while (i < rows) {
        while (j < cols) {
            if (board[i * cols + j] == 0) {
                old_gen[i * cols + j] = 0;
            } else {
                old_gen[i * cols + j] = 1;
            }
        }
    }

    int neighbours;
    while (i < rows) {
        while (j < cols) {
            neighbours = 0;
            for (int k = -1; k < 3; k++){
                for (int l = -1; l < 3; l++){
                    neighbours += old_gen[(i+k) * cols + (j+l)];
                }
            }
            neighbours -= old_gen[i * cols + j];

            if (old_gen[i * cols + j] == 1 && (neighbours < 2 || neighbours > 3)) {
                board[i * cols + j] = 0;
            }
            if (old_gen[i * cols + j] == 0 && neighbours == 3) {
                board[i * cols + j] = 1;
            }
        }
    }
}


void show(int *board, int rows, int cols) {
    clear();
    int i = 0;
    int j = 0;
    while (i < rows) {
        while (j < cols) {
            if (board[i * cols + j] == 0) {
                gotoxy(j, i);
                backgroundblack();
            } else {
                gotoxy(j, i);
                backgroundgreen();
            }
        }
    }
}


int main(int argc, char const *argv[]) {
    int c;

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int rows = w.ws_row;
    int cols = w.ws_col;

    printf("rows: %i\ncols: %i\n", rows, cols);

    // Board init
    int board[rows][cols];
    int old_gen[rows][cols];
    int r;
    int i, j = 0;
    while (i < rows) {
        while (j < cols) {
            srand(time(NULL));
            r = rand();
            if (r < 0.5) {
                board[i][j] = 0;
            } else {
                board[i][j] = 1;
            }
        }
    }
    printf("initialization compleated");

    int *board_p = &board;
    int *old_gen_p = &old_gen;

    // Game Loop
    // int loops = 100;
    // while(loops > 0) {
    //     show(*board_p, rows, cols);
    //     //update(*board_p, *old_gen_p, rows, cols);
    //     loops -= 1;
    // }

    clear();
    return 0;
}