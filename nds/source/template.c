#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// nintendo ds screen size
const int WIDTH = 256;
const int HEIGHT = 192;

// 8*6 sprite of 32*32
// 16*12 sprite of 16*16
// 32*24 sprite of 8*8
const int sprite_size = 32;
const int rows = HEIGHT / sprite_size;
const int cols = WIDTH / sprite_size;

typedef struct {
	u16* gfx;
	SpriteSize size;
	SpriteColorFormat format;
	int color;
	int paletteAlpha;
	int x;
	int y;
} Sprite;

touchPosition touch;

void init(int *board, int *input_board) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
			int state = rand() % 2;
            board[i * cols + j] = state;
            input_board[i * cols + j] = 0;
        }
    }
}

void input_init(int *board, int *input_board) {
	for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (input_board[i * cols + j] == 0) {
                board[i * cols + j] = 0;
            } else {
                board[i * cols + j] = 1;
            }
        }
    }
}

void clear(int *board, int *input_board) {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			board[i * cols + j] = 0;
			input_board[i * cols + j] = 0;
		}
	}
}

void update(int *board, int *old_gen) {
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

int main(int argc, char** argv) {

	srand(time(NULL));

	videoSetMode(MODE_0_2D);
	oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);
	vramSetBankA(VRAM_A_MAIN_SPRITE);
    
	videoSetModeSub(MODE_0_2D);
	oamInit(&oamSub, SpriteMapping_Bmp_1D_128, false);
	vramSetBankD(VRAM_D_SUB_SPRITE);

	int board[rows * cols];
    int old_gen[rows * cols];
    int input_board[rows * cols];

	int (*board_p)[] = &board;
	int (*old_gen_p)[] = &old_gen;
	int (*input_board_p)[] = &input_board;

	int tile_colors[] = {
		ARGB16(1, 0, 0, 0), // black (dead)
		ARGB16(1, 0, 255, 0) // green (alive)
	};

	u16* gfx_main[6*8] = {0};
	u16* gfx_sub[6*8] = {0};

	init(*board_p, *input_board_p);

	while(1) {
		scanKeys();
		if(keysHeld() & KEY_A) {
			init(*board_p, *input_board_p);
		}
		if(keysHeld() & KEY_X) {
			input_init(*board_p, *input_board_p);
		}
		if(keysHeld() & KEY_B) {
			clear(*board_p, *input_board_p);
		}
		if(keysHeld() & KEY_TOUCH) {
            touchRead(&touch); // set touch variable
			int i = (int) touch.py / sprite_size;
			int j = (int) touch.px / sprite_size;
			input_board[i * cols + j] = 1;
        }

		int count = 0;
		for (int i = 0; i < rows; i++) {
        	for (int j = 0; j < cols; j++) {
				count++;

				int main_tile = board[i * cols + j];
				Sprite main_tmp = { 0, SpriteSize_32x32, SpriteColorFormat_Bmp, tile_colors[main_tile], 15, j * sprite_size, i * sprite_size };

				int sub_tile = input_board[i * cols + j];
				Sprite sub_tmp = { 0, SpriteSize_32x32, SpriteColorFormat_Bmp, tile_colors[sub_tile], 15, j * sprite_size, i * sprite_size };

				if (gfx_main[count] == 0) { // you can only allocate once
					gfx_main[count] = oamAllocateGfx(&oamMain, main_tmp.size, main_tmp.format); // allocate some space for the sprite graphics
				}
				if (gfx_sub[count] == 0) { // you can only allocate once
					gfx_sub[count] = oamAllocateGfx(&oamSub, sub_tmp.size, sub_tmp.format); // allocate some space for the sprite graphics
				}

				dmaFillHalfWords(main_tmp.color, gfx_main[count], sprite_size*sprite_size*2); // fill each as a Red Square
				dmaFillHalfWords(sub_tmp.color, gfx_sub[count], sprite_size*sprite_size*2); // fill each as a Red Square

				oamSet(
					&oamMain, //sub display
					count, //oam entry to set
					main_tmp.x, main_tmp.y, //position
					0, //priority
					main_tmp.paletteAlpha, //palette for 16 color sprite or alpha for bmp sprite
					main_tmp.size, main_tmp.format, gfx_main[count], -1,
					false, //double the size of rotated sprites
					false, //don't hide the sprite
					false, false, //vflip, hflip
					false //apply mosaic
				);

				oamSet(
					&oamSub, //sub display
					count, //oam entry to set
					sub_tmp.x, sub_tmp.y, //position
					0, //priority
					sub_tmp.paletteAlpha, //palette for 16 color sprite or alpha for bmp sprite
					sub_tmp.size, sub_tmp.format, gfx_sub[count], -1,
					false, //double the size of rotated sprites
					false, //don't hide the sprite
					false, false, //vflip, hflip
					false //apply mosaic
				);
			}
		}

		update(*board_p, *old_gen_p);
		usleep(1000000); // microseconds

		swiWaitForVBlank();
		oamUpdate(&oamMain);
		oamUpdate(&oamSub);
	}
	//return 0;
}