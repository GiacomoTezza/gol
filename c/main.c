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

static struct termios term, oterm;

void update(int *board, int *old_gen, int rows, int cols) {
    // Copy of the board to a temp board
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (board[i * cols + j] == 0) {
                old_gen[i * cols + j] = 0;
            } else {
                old_gen[i * cols + j] = 1;
            }
        }
    }

    int neighbours;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            neighbours = 0;
            
            for (int k = -1; k < 2; k++){
                for (int l = -1; l < 2; l++){
                    if (!(k+i < 0 || k+i > rows - 1 || l+j < 0 || l+j > cols - 1)) {
                        neighbours += old_gen[(i+k) * cols + (j+l)];
                    }
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
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (board[i * cols + j] == 0) {
                gotoxy(i, j);
                backgroundblack();
                printf(" ");
            } else {
                gotoxy(i, j);
                backgroundgreen();
                printf(" ");
            }
        }
    }
}


int main(int argc, char const *argv[]) {
    struct winsize w;
    
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    const int rows = w.ws_row;
    const int cols = w.ws_col;
    int board[rows * cols];
    int old_gen[rows * cols];

    // Board init
    srand(time(NULL));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            board[i * cols + j] = rand() % 2;
        }
    }

    int (*board_p)[] = &board;
    int (*old_gen_p)[] = &old_gen;

    // // Game Loop
    while(1) {
        show(*board_p, rows, cols);
        update(*board_p, *old_gen_p, rows, cols);
        usleep(50000); // microseconds
    }

    clear();
    return 0;
}