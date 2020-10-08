#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

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


// void show(int *board, int rows, int cols) {
// }


int init(int argc, char const *argv[]) {
    const int rows = 10;
    const int cols = 10;
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
        // show(*board_p, rows, cols);
        update(*board_p, *old_gen_p, rows, cols);
        usleep(50000); // microseconds
    }

    return 0;
}