import pygame
from pygame.locals import *
from random import choice
from copy import deepcopy
from time import sleep


def init(w, h):
    return [[choice([0, 1]) for i in range(w)] for j in range(h)]


def show_cli(board):
    for i in range(len(board)):
        for j in range(len(board[i])):
            print("*" if board[i][j] else " ", end="")
        print()
    print()


def update(board):
    old_gen = deepcopy(board)

    for i in range(len(old_gen)):
        for j in range(len(old_gen[i])):

            neighbours = 0
            for k in range(-1, 2):
                for l in range(-1, 2):
                    try:
                        neighbours += old_gen[i + k][j + l]
                    except:
                        pass

            neighbours -= old_gen[i][j]

            if old_gen[i][j] == 1 and (neighbours < 2 or neighbours > 3):
                board[i][j] = 0
            if old_gen[i][j] == 0 and neighbours == 3:
                board[i][j] = 1

    return board


# if __name__ == "__main__":
#     board = init(32, 32)

#     while True:
#         update(board)
#         show_cli(board)
#         sleep(0.05)

pygame.init()

GAME_RES = WIDTH, HEIGHT = 600, 600
FPS = 144
GAME_TITLE = 'GOL'

window = pygame.display.set_mode(GAME_RES, HWACCEL | HWSURFACE | DOUBLEBUF)
pygame.display.set_caption(GAME_TITLE)
clock = pygame.time.Clock()

# Game Values

background_color = (150, 150, 150)
alive_cell_color = (0, 255, 0)
dead_cell_color = (0, 0, 0)
w, h = WIDTH // 10, HEIGHT // 10
cell_width = WIDTH / w
cell_height = HEIGHT / h
board = init(w, h)

# End of Game Values

# Game loop
game_ended = False
while not game_ended:

    # Event handling
    for event in pygame.event.get():
        if event.type == QUIT:
            game_ended = True
            break
        if event.type == KEYDOWN:
            if event.key == K_ESCAPE:
                game_ended = True
                break
            if event.key == K_RETURN:
                board = init(w, h)
                break

    # Game logic
    update(board)

    # Display update
    #pygame.Surface.fill(window, background_color)

    for i in range(len(board)):
        for j in range(len(board[i])):
            cell_rect = pygame.Rect(j * cell_width, i *
                                    cell_height, cell_width, cell_height)
            pygame.draw.rect(
                window, alive_cell_color if board[i][j] else dead_cell_color, cell_rect)

    # sleep(0.05)

    pygame.display.update()
    clock.tick(FPS)

pygame.quit()
exit(0)
