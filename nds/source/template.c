#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// nintendo ds screen size
const int WIDTH = 256;
const int HEIGHT = 192;

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

int main(int argc, char** argv) {

	videoSetMode(MODE_0_2D);
	oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);
	vramSetBankA(VRAM_A_MAIN_SPRITE);
    
	videoSetModeSub(MODE_0_2D);
	oamInit(&oamSub, SpriteMapping_Bmp_1D_128, false);
	vramSetBankD(VRAM_D_SUB_SPRITE);

	int board[rows * cols];
    int old_gen[rows * cols];
    int input_board[rows * cols];

	srand(time(NULL));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
			int state = rand() % 2;
            board[i * cols + j] = state;
            input_board[i * cols + j] = state;
        }
    }

	int tile_colors[] = {
		ARGB16(1, 0, 0, 0), // black (dead)
		ARGB16(1, 0, 255, 0) // green (alive)
	};

	u16* gfx_array[6*8] = {0};

	while(1) {
		int count = 0;
		for (int i = 0; i < rows; i++) {
        	for (int j = 0; j < cols; j++) {
				count++;
				int tile = board[i * cols + j]; // select the color for selected tile
				Sprite tmp = { 0, SpriteSize_32x32, SpriteColorFormat_Bmp, tile_colors[tile], 15, j * sprite_size, i * sprite_size };

				if (gfx_array[count] == 0) { // you can only allocate once
					gfx_array[count] = oamAllocateGfx(&oamMain, tmp.size, tmp.format); // allocate some space for the sprite graphics
				}

				dmaFillHalfWords(tmp.color, gfx_array[count], sprite_size*sprite_size*2); // fill each as a Red Square

				oamSet(
					&oamMain, //sub display
					count, //oam entry to set
					tmp.x, tmp.y, //position
					0, //priority
					tmp.paletteAlpha, //palette for 16 color sprite or alpha for bmp sprite
					tmp.size, tmp.format, gfx_array[count], -1,
					false, //double the size of rotated sprites
					false, //don't hide the sprite
					false, false, //vflip, hflip
					false //apply mosaic
				);
			}
		}

		swiWaitForVBlank();
		oamUpdate(&oamMain); // send the updates to the hardware
	}
	//return 0;
}