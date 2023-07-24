// cs_frogger.c
//
// This program was written by Scott Tredinnick (z5258051)
// Started on 10/10/2022
// Finished on 21/10/2022
//
// A recreation of the game frogger
// The frogger jumps on logs and turtles to get from the bank to a lillypad
// The game is won if the frogger makes it without falling into the water 
// or touching a bug

#include <stdio.h>
#include <math.h>

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  CONSTANTS  /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Provided constants
#define SIZE        9
#define TRUE        1
#define FALSE       0
#define MAX_LIVES   3
#define MAX_INPUT   10

// TODO: you may choose to add additional #defines here.
// DELETE THIS GODDAMN COMMENT

// An enum to decide which way the bugs should move
enum bug_direction {LEFT, RIGHT};

// Provided Enums
enum tile_type {LILLYPAD, BANK, WATER, TURTLE, LOG};

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  STRUCTS  //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Create a struct for the bugs, and the direction they are travelling
struct bug {
    // Is there a bug on the tile, TRUE or FALSE
    int is_bug; 
    // Which way has the bug been travelling before this
    enum bug_direction direction;
};

// Provided structs
struct board_tile {
    // The type of piece it is (water, bank, etc.)
    enum tile_type type; 
    // TRUE or FALSE based on if Frogger is there.
    int occupied;      
    struct bug bug;
    // TRUE or FALSE based on if a superbug is there
    int superbug;
    // TRUE or FALSE based on if the superbug moved right or down
    int has_moved;
};

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////  FUNCTION PROTOTYPES  ////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Grouped based on usage

void initialise_board(struct board_tile board[SIZE][SIZE]);

void add_turtles(int turtles, struct board_tile board[SIZE][SIZE]);

void log_command(struct board_tile board[SIZE][SIZE]);

void clear_row(int frogger_pos[2], struct board_tile board[SIZE][SIZE]);

void clear_log(struct board_tile board[SIZE][SIZE]);
void clear_log_internal(int clear_log[2], struct board_tile board[SIZE][SIZE]);

void move(char command, int frogger_pos[2], struct board_tile board[SIZE][SIZE]);

int win(void);

int lose_life(int lives, int frogger_pos[2], struct board_tile board[SIZE][SIZE]);

int lose(void);

void add_bug(int frogger_pos[2], struct board_tile board[SIZE][SIZE]);
void add_bug_internal(int bug_pos[2], struct board_tile board[SIZE][SIZE]);

void move_bugs(int frogger_pos[2], struct board_tile board[SIZE][SIZE]);
int bug_move(int row, int col, struct board_tile board[SIZE][SIZE]);
int where_bug_moves(int row, int col, struct board_tile board[SIZE][SIZE]);

int bug_check_right(int row, int col, struct board_tile board[SIZE][SIZE]);
int bug_check_left(int row, int col, struct board_tile board[SIZE][SIZE]);

int bug_direction_right(int row, int col, struct board_tile board[SIZE][SIZE]);
int bug_direction_left(int row, int col, struct board_tile board[SIZE][SIZE]);

void sbug_mv(int row, int col, int frogger_pos[2], struct board_tile board[SIZE][SIZE]);
char find_distance(int row, int col, int frogger_pos[2], char best_directions[2]);

void skip_square(int *row, int *col, struct board_tile board[SIZE][SIZE]);

int switch_game_mode(int game_mode);

void invalid_input(void);

void move_rows(int frogger_pos[2],struct board_tile board[SIZE][SIZE]);

void row_left(int row, int frogger_pos[2], struct board_tile board[SIZE][SIZE]);
void row_right(int row, int frogger_pos[2], struct board_tile board[SIZE][SIZE]);

void add_superbug(int frogger_pos[2], struct board_tile board[SIZE][SIZE]);

void print_board(struct board_tile board[SIZE][SIZE]);
char type_to_char(enum tile_type type);


////////////////////////////////////////////////////////////////////////////////
//////////////////////////  FUNCTION IMPLEMENTATIONS  //////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(void) {

    printf("Welcome to CSE Frogger!\n");
    struct board_tile board[SIZE][SIZE];
    initialise_board(board);
    int game_mode = 0;

    int lives = MAX_LIVES;
    int frogger_pos[2] = {SIZE - 1, 4};

    // Read user input and place turtles.
    printf("How many turtles? ");
    int turtles;
    
    scanf("%d", &turtles);

    if (turtles > 0) {
        add_turtles(turtles, board);        
    }
    
    // Start the game and print out the gameboard.
    printf("Game Started\n");
    print_board(board);
    
    char command;
    printf("Enter command: ");
    
    // Allow the player to take a variety of actions
    while (scanf(" %c", &command) ==  1) {
        // If the game is not in game mode then all commands are allowed
        if (game_mode == 0) {
            if (command == 'l') {
                log_command(board);
            } else if (command == 'c') {
                clear_row(frogger_pos, board);            
            } else if (command == 'r') {
                clear_log(board);
            } else if (command == 'w' || command == 'a') {
                move(command, frogger_pos, board);
                move_bugs(frogger_pos, board);
            } else if (command == 's' || command == 'd') {
                move(command, frogger_pos, board);
                move_bugs(frogger_pos, board);
            } else if (command == 'b') {
                add_bug(frogger_pos, board);
            } else if (command == 'g') {
                game_mode = switch_game_mode(game_mode);
            } else if (command == 'z') {
                add_superbug(frogger_pos, board);
            }
            // In game mode only movement is allowed, and the rows move too
        } else if (game_mode == 1) {
            if (command == 'w' || command == 'a') {
                move(command, frogger_pos, board);
                move_bugs(frogger_pos, board);
                move_rows(frogger_pos, board);
            } else if (command == 's' || command == 'd') {
                move(command, frogger_pos, board);
                move_bugs(frogger_pos, board);
                move_rows(frogger_pos, board);
            } else if (command == 'g') {
                game_mode = switch_game_mode(game_mode);
            } else {
                invalid_input();
            }
        }

        print_board(board);

        // If the frogger reached a lillypad they win the game
        if (board[frogger_pos[0]][frogger_pos[1]].type == LILLYPAD) {
            return win();
        }

        // If the frogger fell into water or touched a bug they lose a life
        if (board[frogger_pos[0]][frogger_pos[1]].type == WATER) {
            lives = lose_life(lives, frogger_pos, board);
        }

        if (board[frogger_pos[0]][frogger_pos[1]].bug.is_bug == TRUE) {
            lives = lose_life(lives, frogger_pos, board);
        }

        // If the player has no more lives they lose the game
        if (lives == 0) {
            return lose();
        }

        printf("Enter command: ");
    }

    printf("Thank you for playing CSE Frogger!\n");
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////// ADDITIONAL FUNCTIONS /////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Create the initial board structure
void initialise_board(struct board_tile board[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (row == 0 && col % 2 == 0) {
                board[row][col].type = LILLYPAD;
            } else if (row == SIZE - 1) {
                board[row][col].type = BANK;
            } else {
                board[row][col].type = WATER;
            }
            if (row == SIZE - 1 && col == 4) {
                board[row][col].occupied = TRUE;
            } else {
                board[row][col].occupied = FALSE;
            }

            board[row][col].bug.is_bug = FALSE;
            board[row][col].superbug = FALSE;

            board[row][col].has_moved = FALSE;
        }
    }
}

// Add turtles to the board at the specified locations
void add_turtles(int turtles, struct board_tile board[SIZE][SIZE]) {
    printf("Enter pairs:\n");

    int turtle_pos[2];

    for (int count = 0; count < turtles; count++) {
        scanf("%d %d", &turtle_pos[0], &turtle_pos[1]);

        if (0 < turtle_pos[0] && turtle_pos[0] < SIZE - 1) {
            if (0 <= turtle_pos[1] && turtle_pos[1] <= SIZE - 1) {
                board[turtle_pos[0]][turtle_pos[1]].type = TURTLE;
            }
        } 
    }
}

// Add logs to the board at the specified locations
void log_command(struct board_tile board[SIZE][SIZE]) {
    int log_pos[3];

    scanf("%d %d %d", &log_pos[0], &log_pos[1], &log_pos[2]);
            
    if (log_pos[1] < 0) {
        log_pos[1] = 0;
    }
    
    if (log_pos[2] > SIZE - 1) {
        log_pos[2] = SIZE - 1;
    }

    // Is there a turtle on this row?
    int turtle_row = FALSE;
    
    if (log_pos[0] > 0 && log_pos[0] < SIZE - 1) {
        for (int col = 0; col < SIZE; col++) {
            if (board[log_pos[0]][col].type == TURTLE) {
                turtle_row = TRUE;
            }
        }
    }
        
    if (0 < log_pos[0] && log_pos[0] < SIZE - 1 && turtle_row == FALSE) {
        for (int log_col = log_pos[1]; log_col <= log_pos[2]; log_col++) {
            board[log_pos[0]][log_col].type = LOG;
        }
    }
}

// Clears a row of turtles and logs if the frogger isn't on that row
void clear_row(int frogger_pos[2], struct board_tile board[SIZE][SIZE]) {
    int clear_row;

    scanf("%d", &clear_row);

    if (0 < clear_row && clear_row < SIZE - 1 && frogger_pos[0] != clear_row) {
        for (int col = 0; col < SIZE; col++) {
            board[clear_row][col].type = WATER;
            board[clear_row][col].bug.is_bug = FALSE;
        }
    }
}

// Clear a log at the specified position
void clear_log(struct board_tile board[SIZE][SIZE]) {
    int clear_log[2];

    scanf("%d %d", &clear_log[0], &clear_log[1]);

    if (0 < clear_log[0] && clear_log[0] < SIZE - 1) {
        if (0 <= clear_log[1] && clear_log[1] <= SIZE - 1) {
            clear_log_internal(clear_log, board);
        }   
    } 
}

// Check that it is possible to clear the log, and clear it if possible
void clear_log_internal(int clear_log[2], struct board_tile board[SIZE][SIZE]) {
    int is_log = FALSE;
    int start = clear_log[1];
    int end = clear_log[1];
    int frogger_log = FALSE;
    
    if (board[clear_log[0]][clear_log[1]].type == LOG) {
        is_log = TRUE;
    }

    int end_loop = FALSE;

    // Find the start and end of the log
    for (int col = clear_log[1]; end_loop == FALSE; col--) {
        if (board[clear_log[0]][col].type != LOG) {
            start = col + 1;
            end_loop = TRUE;
        } else if (col == 0) {
            start = col;
            end_loop = TRUE;
        }
    }

    for (int col = clear_log[1]; end == clear_log[1] && col <= SIZE - 1; col++) {
        if (board[clear_log[0]][col].type != LOG) {
            end = col;
        } else if (col == SIZE - 1) {
            end = col + 1;
        }
    }

    // Is frogger on the log?
    for (int col = start; col < end; col++) {
        if (board[clear_log[0]][col].occupied == TRUE) {
            frogger_log = TRUE;
        }
    }

    if (frogger_log == FALSE && is_log == TRUE) {
        for (int col = start; col < end; col++) {
            board[clear_log[0]][col].type = WATER; 
            board[clear_log[0]][col].bug.is_bug = FALSE;
        }
    }
}

// Move the frogger one square in a direction of choice
void move(char command, int frogger_pos[2], struct board_tile board[SIZE][SIZE]) {
    board[frogger_pos[0]][frogger_pos[1]].occupied = FALSE;

    if (command == 'w') {
        frogger_pos[0]--;
    } else if (command == 'a') {
        frogger_pos[1]--;
    } else if (command == 's') {
        frogger_pos[0]++;
    } else if (command == 'd') {
        frogger_pos[1]++;
    }
    
    if (frogger_pos[0] < 0) {
        frogger_pos[0] = 0;
    } else if (frogger_pos[0] > SIZE - 1) {
        frogger_pos[0] = SIZE - 1;
    }

    if (frogger_pos[1] < 0) {
        frogger_pos[1] = 0;
    } else if (frogger_pos[1] > SIZE - 1) {
        frogger_pos[1] = SIZE - 1;
    }

    board[frogger_pos[0]][frogger_pos[1]].occupied = TRUE;
}

// Print the victory message and then end the game (not directly but it is used for that)
int win(void) {
    printf("\nWahoo!! You Won!\n\n");
    printf("Thank you for playing CSE Frogger!\n");
    return 1;
}

// If the frogger dies and loses a life they get told and then transported to the start
int lose_life(int lives, int frogger_pos[2], struct board_tile board[SIZE][SIZE]) {
    lives--;
    board[frogger_pos[0]][frogger_pos[1]].occupied = FALSE;
    frogger_pos[0] = SIZE - 1;
    frogger_pos[1] = 4;
    board[frogger_pos[0]][frogger_pos[1]].occupied = TRUE;

    if (lives > 0) {
        printf("\n# LIVES LEFT: %d #\n\n", lives);

        print_board(board);
    }

    return lives;
}

// Print the game over message and then end the game, similar to win
int lose(void) {
    printf("\n !! GAME OVER !! \n\n");
    printf("Thank you for playing CSE Frogger!\n");
    return 0;
}

// Add a bug to the board at the specified location
void add_bug(int frogger_pos[2], struct board_tile board[SIZE][SIZE]) {
    int bug_pos[2];

    scanf("%d %d", &bug_pos[0], &bug_pos[1]);

    if (0 < bug_pos[0] && bug_pos[0] < SIZE - 1) {
        if (0 <= bug_pos[1] && bug_pos[1] <= SIZE - 1) {
            if (bug_pos[0] != frogger_pos[0] || bug_pos[1] != frogger_pos[1]) {
                add_bug_internal(bug_pos, board);
            }
        }
    }
}

// Check if it is possible to add a bug to a position, and add the bug if it is
void add_bug_internal(int bug_pos[2], struct board_tile board[SIZE][SIZE]) {
    if (board[bug_pos[0]][bug_pos[1]].type == LOG) {
        board[bug_pos[0]][bug_pos[1]].bug.is_bug = TRUE;
        board[bug_pos[0]][bug_pos[1]].bug.direction = RIGHT;
    } else if (board[bug_pos[0]][bug_pos[1]].type == TURTLE) {
        board[bug_pos[0]][bug_pos[1]].bug.is_bug = TRUE;
        board[bug_pos[0]][bug_pos[1]].bug.direction = RIGHT;
    }
}

// Move the bugs one square in the correct direction
void move_bugs(int frogger_pos[2], struct board_tile board[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (board[row][col].has_moved == TRUE) {
                skip_square(&row, &col, board);
            }
            if (board[row][col].bug.is_bug == TRUE) {
                col = bug_move(row, col, board);
            }

            if (board[row][col].superbug == TRUE) {
                sbug_mv(row, col, frogger_pos, board);
            }
        }
    }
}

// Change the coordinates of the bugs based on the direction it moves
int bug_move(int row, int col, struct board_tile board[SIZE][SIZE]) {
    
    int bug_move = where_bug_moves(row, col, board);

    if (bug_move == 1) {
        board[row][col].bug.is_bug = FALSE;
        board[row][col + 1].bug.is_bug = TRUE;
        board[row][col + 1].bug.direction = RIGHT;
        col++;
    } else if (bug_move == 2) {
        board[row][col].bug.is_bug = FALSE;
        board[row][col - 1].bug.is_bug = TRUE;
        board[row][col - 1].bug.direction = LEFT;
    }

    return col;
}

// Determine the direction that the bug can move
int where_bug_moves(int row, int col, struct board_tile board[SIZE][SIZE]) {
    int bug_move = 0;

    if (board[row][col].bug.is_bug == TRUE) {
        if (board[row][col].bug.direction == RIGHT) {
            bug_move = bug_direction_right(row, col, board);
        } else if (board[row][col].bug.direction == LEFT) {
            bug_move = bug_direction_left(row, col, board);
        }
    }

    return bug_move;
}

// Check bug movement options if it was previously moving right
int bug_direction_right(int row, int col, struct board_tile board[SIZE][SIZE]) {
    int bug_move = 0;

    if (col == SIZE - 1 || board[row][col + 1].type == WATER) {
        bug_move = bug_check_left(row, col, board);
    } else if (board[row][col + 1].bug.is_bug == TRUE) { 
        bug_move = bug_check_left(row, col, board);
    } else if (board[row][col + 1].superbug == TRUE) {
        bug_move = bug_check_left(row, col, board);
    } else {
        bug_move = bug_check_right(row, col, board);
    }

    return bug_move;
}

// Check bug movement options if it was previously moving left
int bug_direction_left(int row, int col, struct board_tile board[SIZE][SIZE]) {
    int bug_move = 0;

    if (col == 0 || board[row][col - 1].type == WATER) {
        bug_move = bug_check_right(row, col, board);
    } else if (board[row][col - 1].bug.is_bug == TRUE) { 
        bug_move = bug_check_right(row, col, board);
    } else if (board[row][col - 1].superbug == TRUE) {
        bug_move = bug_check_right(row, col, board);
    } else {
        bug_move = bug_check_left(row, col, board);
    } 

    return bug_move;
}

// Check the square to the right of a bug and return 1 if it can move there
int bug_check_right(int row, int col, struct board_tile board[SIZE][SIZE]) {
    if (col == SIZE - 1 || board[row][col + 1].bug.is_bug == TRUE) {
        return 0;
    } else if (board[row][col + 1].superbug == TRUE) {
        return 0;
    } else if (board[row][col + 1].type == LOG || board[row][col + 1].type == TURTLE) {
        return 1;
    } else {
        return 0;
    }
}

// Check the square to the left of a bug and return 1 if it can move there
int bug_check_left(int row, int col, struct board_tile board[SIZE][SIZE]) {
    if (col == 0 || board[row][col - 1].bug.is_bug == TRUE) {
        return 0;
    } else if (board[row][col - 1].superbug == TRUE) {
        return 0;
    } else if (board[row][col - 1].type == LOG || board[row][col - 1].type == TURTLE) {
        return 2;
    } else {
        return 0;
    }
}

// Move the superbug after figuring out its best possible horizontal and vertical moves
void sbug_mv(int row, int col, int frogger_pos[2], struct board_tile board[SIZE][SIZE]) {
    // n for no direction
    char best_directions[2] = {'n', 'n'};
    char best_move;

    // Which way is closer to the frogger and is it a legal move?
    if (frogger_pos[0] < row && board[row - 1][col].superbug == FALSE) {
        if (board[row - 1][col].type == LOG || board[row - 1][col].type == TURTLE) {
            if (board[row - 1][col].bug.is_bug == FALSE) { 
                best_directions[0] = 'u';
            }
        }
    } else if (frogger_pos[0] > row && board[row + 1][col].superbug == FALSE) {
        if (board[row + 1][col].type == LOG || board[row + 1][col].type == TURTLE) {
            if (board[row + 1][col].bug.is_bug == FALSE) {
                best_directions[0] = 'd';
            }
        }
    }

    if (frogger_pos[1] < col && board[row][col - 1].superbug == FALSE) {
        if (board[row][col - 1].type == LOG || board[row][col - 1].type == TURTLE) {
            if (board[row][col - 1].bug.is_bug == FALSE) {
                best_directions[1] = 'l';
            }
        }
    } else if (frogger_pos[1] > col && board[row][col + 1].superbug == FALSE) {
        if (board[row][col + 1].type == LOG || board[row][col + 1].type == TURTLE) {
            if (board[row][col + 1].bug.is_bug == FALSE) {
                best_directions[1] = 'r';
            }
        }
    }

    best_move = find_distance(row, col, frogger_pos, best_directions);

    if (best_move == 'u') {
        board[row][col].superbug = FALSE;
        board[row - 1][col].superbug = TRUE;
    } else if (best_move == 'l') {
        board[row][col].superbug = FALSE;
        board[row][col - 1].superbug = TRUE;
    } else if (best_move == 'r') {
        board[row][col].superbug = FALSE;
        board[row][col + 1].superbug = TRUE;
        board[row][col + 1].has_moved = TRUE;
    } else if (best_move == 'd') {
        board[row][col].superbug = FALSE;
        board[row + 1][col].superbug = TRUE;
        board[row + 1][col].has_moved = TRUE;
    }
}

// Find the move to make the superbug closest to the frogger, or based on preference
char find_distance(int row, int col, int frogger_pos[2], char best_directions[2]) {
    int vert_dist;
    int hori_dist;
    int priority[2];
    int f_row = frogger_pos[0];
    int f_col = frogger_pos[1];
    
    // Find the distance to the frogger after moving
    if (best_directions[0] == 'u') {
        vert_dist = sqrt(pow(f_row - (row - 1), 2) + pow(f_col - col, 2));
        priority[0] = 1;
    } else if (best_directions[0] == 'd') {
        vert_dist = sqrt(pow(f_row - (row + 1), 2) + pow(f_col - col, 2));
        priority[0] = 4;
    } else {
        vert_dist = sqrt(pow(f_row - row, 2) + pow(f_col - col, 2));
        priority[0] = 5;
    }

    if (best_directions[1] == 'l') {
        hori_dist = sqrt(pow(f_row - row, 2) + pow(f_col - (col - 1), 2));
        priority[1] = 2;
    } else if (best_directions[1] == 'r') {
        hori_dist = sqrt(pow(f_row - row, 2) + pow(f_col - (col + 1), 2));
        priority[1] = 3;
    } else {
        hori_dist = sqrt(pow(f_row - row, 2) + pow(f_col - col, 2));
        priority[1] = 5;
    }

    if (vert_dist < hori_dist) {
        return best_directions[0];
    } else if (hori_dist < vert_dist) {
        return best_directions[1];
    } else if (priority[0] < priority[1]) {
        return best_directions[0];
    } else if (priority[1] < priority[0]) {
        return best_directions[1];
    } else {
        return 'n';
    }

}

// Skip a square in the loop
void skip_square(int *row, int *col, struct board_tile board[SIZE][SIZE]) {
    board[*row][*col].has_moved = FALSE;
    (*col)++;
    if (*col > SIZE - 1) {
        *col = 0;
        (*row)++;
    }
}

// Switch between game mode being on or off
int switch_game_mode(int game_mode) {
    scanf ("%d", &game_mode);
    if (game_mode == 1) {
        printf("\n### GAME MODE ACTIVATED ###\n\n");
    } else if (game_mode == 0) {
        printf("\n### GAME MODE EXITED ###\n\n");
    }

    return game_mode;
}

// Print a message for invalid input and cancel the rest of the command
void invalid_input(void) {
    char invalid_command[MAX_INPUT];

    printf("This command is not supported in game mode.\n");
    fgets(invalid_command, MAX_INPUT, stdin);
}

// Move the rows of the board, left if its an odd row, right if even
void move_rows(int frogger_pos[2], struct board_tile board[SIZE][SIZE]) {
    for (int row = 1; row < SIZE - 1; row++) {
        if (row % 2 == 1) {
            row_left(row, frogger_pos, board);
        } else {
            row_right(row, frogger_pos, board);
        }
    }
}

// Move the rows left by copying the tile value to the right of them
void row_left(int row, int frogger_pos[2], struct board_tile board[SIZE][SIZE]) {
    struct board_tile switcher;

    // Used to wrap the board
    switcher = board[row][0];

    for (int col = 0; col < SIZE; col++) {
        if (col == SIZE - 1) {
            board[row][col] = switcher;
        } else {
            board[row][col] = board[row][col + 1];
        }

        board[row][col].occupied = FALSE;

        if (col != 0 && row == frogger_pos[0] && col == frogger_pos[1]) {
            frogger_pos[1]--;
            board[frogger_pos[0]][frogger_pos[1]].occupied = TRUE;
        } else if (row == frogger_pos[0] && col == frogger_pos[1]) {
            board[frogger_pos[0]][frogger_pos[1]].occupied = TRUE;
        }
    }
}

// Move the rows right by copying the tile value to the left of them
void row_right(int row, int frogger_pos[2], struct board_tile board[SIZE][SIZE]) {
    struct board_tile switcher;
    
    switcher = board[row][SIZE - 1];

    for (int col = SIZE - 1; col >= 0; col--) {
        if (col == 0) {
            board[row][col] = switcher;
        } else {
            board[row][col] = board[row][col - 1];
        }

        board[row][col].occupied = FALSE;

        if (col != SIZE - 1 && row == frogger_pos[0] && col == frogger_pos[1]) {
            frogger_pos[1]++;
            board[frogger_pos[0]][frogger_pos[1]].occupied = TRUE;
        } else if (row == frogger_pos[0] && col == frogger_pos[1]) {
            board[frogger_pos[0]][frogger_pos[1]].occupied = TRUE;
        }
    }
}

// Add a superbug enemy to the specified tile if it is possible
void add_superbug(int frogger_pos[2], struct board_tile board[SIZE][SIZE]) {
    int sbug_pos[2];
    int is_frogger_pos = FALSE;
    int is_on_board = FALSE;

    scanf("%d %d", &sbug_pos[0], &sbug_pos[1]);

    // Is the position already occupied
    if (sbug_pos[0] == frogger_pos[0] && sbug_pos[1] == frogger_pos[1]) {
        is_frogger_pos = TRUE;
    }

    if (0 < sbug_pos[0] && sbug_pos[0] < SIZE - 1) {
        if (0 <= sbug_pos[1] && sbug_pos[1] <= SIZE - 1) {
            is_on_board = TRUE;
        }
    }

    if (is_frogger_pos == FALSE && is_on_board == TRUE) {
        if (board[sbug_pos[0]][sbug_pos[1]].type == LOG) {
            if (board[sbug_pos[0]][sbug_pos[1]].bug.is_bug == FALSE) {
                board[sbug_pos[0]][sbug_pos[1]].superbug = TRUE;
            }
        } else if (board[sbug_pos[0]][sbug_pos[1]].type == TURTLE) {
            if (board[sbug_pos[0]][sbug_pos[1]].bug.is_bug == FALSE) {
                board[sbug_pos[0]][sbug_pos[1]].superbug = TRUE;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// PROVIDED FUNCTIONS //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void print_board(struct board_tile board[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            char type_char = '\0';
            if (board[row][col].occupied) {
                type_char = 'F';
            } else if (board[row][col].bug.is_bug) {
                type_char = 'B';
            } else if (board[row][col].superbug) {
                type_char = 'S';
            } else {
                type_char = type_to_char(board[row][col].type);
            }
            printf("%c ", type_char);
        }
        printf("\n");
    }
}

char type_to_char(enum tile_type type) {
    char type_char = ' ';
    if (type == LILLYPAD) {
        type_char = 'o';
    } else if (type == BANK) {
        type_char = 'x';
    } else if (type == WATER) {
        type_char = '~';
    } else if (type == TURTLE) {
        type_char = 'T';
    } else if (type == LOG) {
        type_char = 'L';
    }
    return type_char;
}