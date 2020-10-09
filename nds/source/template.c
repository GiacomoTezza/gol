#include <nds.h>
#include <stdio.h>

// nintendo ds screen size
const int WIDTH = 256;
const int HEIGHT = 192;

// 16*12 sprite of 16*16
// 32*24 sprite of 8*8
const int sprite_size = 16;
const int rows = HEIGHT / sprite_size;
const int cols = WIDTH / sprite_size;

// sprite struct
typedef struct {
	u16* gfx;
	int color;
	int x, y;
} Sprite;

Sprite mainBoard[rows * cols];
Sprite subBoard[rows * cols];

touchPosition touch;
void addNewSquare(int j, int i, int c, int s);
void createSquare(Sprite tmp_sprite, int count, OamState* screen);

int main(void) {  
    int board[rows * cols];
    int old_gen[rows * cols];
    int input_board[rows * cols];

    // u16* mainGFX = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_Bmp);
	// u16* subGFX = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_Bmp);

    // board init
    srand(time(NULL));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            board[i * cols + j] = 0;
            input_board[i * cols + j] = 0;
			addNewSquare(j, i, 0, 0); // add sprite in mainBoard
			addNewSquare(j, i, 0, 1); // add sprite in subBoard
        }
    }

	// Set up the top screen
	videoSetMode(MODE_0_2D);
	vramSetBankA(VRAM_A_MAIN_SPRITE);
	// set up the bottom screen
	videoSetModeSub(MODE_0_2D);
	vramSetBankD(VRAM_D_SUB_SPRITE);

	consoleDemoInit();

	oamInit(&oamMain, SpriteMapping_1D_32, false);
	oamInit(&oamSub, SpriteMapping_1D_32, false);


	while(1) {
		// scan for touch
		scanKeys();
		int key = keysHeld();

		// select cells to turn on
		if(key & KEY_TOUCH) {
            touchRead(&touch); // set touch variable
			int i = (int) touch.py / sprite_size;
			int j = (int) touch.px / sprite_size;
			input_board[i * cols + j] = 1;
        }

		// createSquare(touch.px, touch.py, &oamMain, mainGFX, ARGB16(1, 31, 12, 12));
		
		// draw selection on touchscreen
		for (int i = 0; i < rows; i++) {
        	for (int j = 0; j < cols; j++) {
				mainBoard[i * cols + j].color = input_board[i * cols + j];
				createSquare(mainBoard[i * cols + j], i * cols + j, &oamSub);
			}
		}

		// draw screen
		swiWaitForVBlank(); // prints the screen
		// update oam
		oamUpdate(&oamSub); // (sub) updates the oam before so VBlank can update the graphics on screen
		oamUpdate(&oamMain); // (main) updates the oam before so VBlank can update the graphics on screen
	}
	return 0;
}

// adds new square to memory
void addNewSquare(int j, int i, int c, int s) {
	int color = ARGB16(1, 0, 255 * c, 0);	
	int x = sprite_size * j;
	int y = sprite_size * i;


	Sprite tmp = {0, color, x, y};
	if (s) {
		tmp.gfx = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_Bmp);
		subBoard[i * cols + j] = tmp;
	} else {
		tmp.gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_Bmp);
		mainBoard[i * cols + j] = tmp;
	}
}

// createSquare is a function that easily allows us to add a sprite on the screen with various properties.
void createSquare(Sprite tmp_sprite, int count, OamState* screen) {
	dmaFillHalfWords(tmp_sprite.color, tmp_sprite.gfx, 16*16*2); // fill each square with correct color

	oamSet(
		screen, // sub display
		count, // oam entry to set
		tmp_sprite.x, tmp_sprite.y, //position
		0, // priority
		15, // palette for 16 color sprite or alpha for bmp sprite
		SpriteSize_16x16,
		SpriteColorFormat_Bmp,
		tmp_sprite.gfx,
		0, // rotation
		true, // double the size of rotated sprites
		false, // don't hide the sprite
		false, false, // vflip, hflip
		false // apply mosaic
	);
}
