// Height and width of the playing area.
#define RESULT_SIZE 40
#define UNDO 0
#define TEMP_ROW 1
#define TEMP_COL 2
typedef struct{
    char board[50];
    int undo;
    int temp_row;
    int temp_col;
} GameState;