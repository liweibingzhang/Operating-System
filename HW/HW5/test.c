#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOARD_SIZE 10
#define DICT_SIZE 10
#define WORD_SIZE 6

int is_valid(char board[BOARD_SIZE][BOARD_SIZE], char **dict, int dict_size) {
    char word[WORD_SIZE + 1];
    int i, j, k, l, found;
    // check rows
    for (i = 0; i < BOARD_SIZE; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == 0) continue;
            for (k = 0, l = j; l < BOARD_SIZE && k < WORD_SIZE; k++, l++) {
                word[k] = board[i][l];
            }
            word[k] = '\0';
            found = 0;
            for (k = 0; k < dict_size; k++) {
                if (strcmp(dict[k], word) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) return 0;
        }
    }
    // check columns
    for (j = 0; j < BOARD_SIZE; j++) {
        for (i = 0; i < BOARD_SIZE; i++) {
            if (board[i][j] == 0) continue;
            for (k = 0, l = i; l < BOARD_SIZE && k < WORD_SIZE; k++, l++) {
                word[k] = board[l][j];
            }
            word[k] = '\0';
            found = 0;
            for (k = 0; k < dict_size; k++) {
                if (strcmp(dict[k], word) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) return 0;
        }
    }
    return 1;
}

int main() {
    char board[BOARD_SIZE][BOARD_SIZE] = {
        {0, 't', 'e', 's', 't', 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        //{0, 0, 0, 'l', 'e', 'n', 't', 'o', 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        //{0, 0, 0, 0, 0, 'h', 'e', 'l', 'p', 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        //{0, 0, 0, 0, 'a', 's', 'k', 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},     
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};
char *dict[DICT_SIZE] = {"help","test", "hello", "world", "person", "exit","lento","lead","ask","bit"};

if (is_valid(board, dict, DICT_SIZE)) {
    printf("All words on the board are in the dictionary.\n");
} else {
    printf("There is a word on the board that is not in the dictionary.\n");
}

return 0;
}

