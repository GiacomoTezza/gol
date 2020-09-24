from random import choice
from copy import deepcopy
from time import sleep


def init(w, h):
    return [[choice([0, 1]) for i in range(w)] for j in range(h)]


def show(board):
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
                    neighbours += old_gen[i + k][j + l]

            neighbours -= old_gen[i][j]

            if old_gen[i][j] == 1 and (neighbours < 2 or neighbours > 3):
                board[i][j] = 0
            if old_gen[i][j] == 0 and neighbours == 3:
                board[i][j] = 1

    return board


if __name__ == "__main__":
    board = init(32, 32)

    while True:
        update(board)
        show(board)
        sleep(0.05)
