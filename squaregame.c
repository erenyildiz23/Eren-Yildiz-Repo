#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

#define BOARD_SIZE 7

char board[BOARD_SIZE][BOARD_SIZE];
int player;     // 1 or 2
int p1_x, p1_y; // Position of player 1
int p2_x, p2_y; // Position of player 2
int turn_id = 0;

// Function prototypes
void initialize_board();
void print_board();
bool is_valid_move(int x, int y);
void make_move(int x, int y);
bool check_win(int x, int y);
void remove_tile(int x, int y);
int minimax(char board[BOARD_SIZE][BOARD_SIZE], int depth, int alpha, int beta, bool maximizingPlayer);
int evaluate_board();
void ai_move();
void ai_remove_tile();
void write_move(int move_x, int move_y, int remove_x, int remove_y);
void read_move(int *move_x, int *move_y, int *remove_x, int *remove_y);
int char_to_index(char c);
int MIN(int a, int b);
int MAX(int a, int b);

int MIN(int a, int b)
{
    return (a < b) ? a : b;
}

int MAX(int a, int b)
{
    return (a > b) ? a : b;
}

int main()
{
    initialize_board();
    print_board();

    while (true)
    {
        if (player == 1)
        {
            ai_move();
            ai_remove_tile();
            printf("AI move: %d %d\n", p1_x, p1_y);
        }
        else
        {
            int x, y;
            while (true)
            {
                // Prompt the human player for their move
                printf("Player %d's turn. Enter move (x y): ", player);

                scanf("%d%d", &y, &x);

                if (is_valid_move(x - 1, y - 1))
                {
                    // The move is valid, so make it
                    make_move(x - 1, y - 1);
                    break;
                }
                else
                {
                    // The move is invalid, so prompt the player to try again
                    printf("Invalid move. Please try again.\n");
                }
            }

            // Prompt the player to remove a tile
            printf("Enter position to remove a tile (x y): ");
            int remove_x, remove_y;
            scanf("%d%d", &remove_y, &remove_x);
            remove_tile(remove_x - 1, remove_y - 1);
            write_move(x, y, remove_x, remove_y);
        }

        print_board();
        if (check_win(p1_x, p1_y) || check_win(p2_x, p2_y))
        {
            printf("Player %d wins!\n", player);
            break;
        }
        // Switch to the other player
        player = (player == 1) ? 2 : 1;
        turn_id++;
    }

    return 0;
}

void initialize_board()
{
    // Initialize the board with empty spaces
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            board[i][j] = ' ';
        }
    }

    // Place player 1 and player 2 on the board
    p1_x = 0;
    p1_y = 3;
    board[p1_x][p1_y] = 'B';

    p2_x = 6;
    p2_y = 3;
    board[p2_x][p2_y] = 'R';

    player = 1; // Player 1 goes first
}

void print_board()
{
    printf("  1 2 3 4 5 6 7\n");
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        printf("%d ", i + 1);
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

// Check if the given move is valid
bool is_valid_move(int x, int y)
{
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE)
    {
        // The move is off the board
        return false;
    }

    if (board[x][y] != ' ')
    {
        // The space is already occupied
        return false;
    }

    if (player == 1)
    {
        // Check the distance from player 1's current position
        if (abs(p1_x - x) > 1 || abs(p1_y - y) > 1)
        {
            // The move is too far away
            return false;
        }
    }
    else
    {
        // Check the distance from player 2's current position
        if (abs(p2_x - x) > 1 || abs(p2_y - y) > 1)
        {
            // The move is too far away
            return false;
        }
    }

    return true;
}
void remove_tile(int x, int y)
{
    // Check if the given position is on the board
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE)
    {
        printf("Invalid position.\n");
        return;
    }

    // Check if the tile is already occupied
    if (board[x][y] != ' ')
    {
        printf("Tile is already occupied.\n");
        return;
    }

    // Mark the tile as removed
    board[x][y] = 'X';
}

bool check_win(int x, int y)
{
    // Check if there are any empty spaces around the player's piece
    bool has_empty_neighbors = false;
    for (int i = x - 1; i <= x + 1; i++)
    {
        for (int j = y - 1; j <= y + 1; j++)
        {
            if (i >= 0 && i < BOARD_SIZE && j >= 0 && j < BOARD_SIZE && board[i][j] == ' ')
            {
                has_empty_neighbors = true;
                break;
            }
        }
    }

    return !has_empty_neighbors;
}

void make_move(int x, int y)
{
    if (!is_valid_move(x, y))
    {
        printf("Invalid move.\n");
        return;
    }

    if (player == 1)
    {
        // remov the old position
        board[p1_x][p1_y] = ' ';

        // Update player 1's position and the board
        p1_x = x;
        p1_y = y;
        board[x][y] = 'B';
    }
    else
    {

        // remov the old position
        board[p2_x][p2_y] = ' ';
        // Update player 2's position and the board
        p2_x = x;
        p2_y = y;
        board[x][y] = 'R';
    }
}
// Determine the best move for the AI player using the minimax algorithm with alpha-beta pruning
void ai_move()
{
    int bestScore = INT_MIN;        // Initialize the best score to the lowest possible value
    int bestRow = -1, bestCol = -1; // Initialize the best move to an invalid position

    // Generate a list of possible moves for the AI player
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (board[i][j] == ' ' && is_valid_move(i, j))
            {
                // Make the move and search the resulting board state
                board[i][j] = 'B';
                int score = minimax(board, 4, INT_MIN, INT_MAX, false);
                board[i][j] = ' '; // Undo the move

                // Update the best score and move if necessary
                if (score > bestScore)
                {
                    bestScore = score;
                    bestRow = i;
                    bestCol = j;
                }
            }
        }
    }

    // Make the best move
    if (bestRow != -1 && bestCol != -1)
    {
        board[p1_x][p1_y] = ' ';
        board[bestRow][bestCol] = 'B';
        p1_x = bestRow;
        p1_y = bestCol;
    }
}
int minimax(char board[BOARD_SIZE][BOARD_SIZE], int depth, int alpha, int beta, bool maximizingPlayer)
{
    // Check if the game is over or the maximum search depth has been reached
    if (check_win(p1_x, p1_y) || check_win(p2_x, p2_y) || depth == 0)
    {
        // Return the score for the current board state
        return evaluate_board();
    }
    if (maximizingPlayer)
    {
        // The AI player is the maximizing player
        int bestScore = INT_MIN; // Initialize the best score to the lowest possible value

        // Generate a list of possible moves for the AI player
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                if (board[i][j] == ' ' && is_valid_move(i, j))
                {
                    // Make the move and recursively search the resulting board state
                    board[i][j] = 'B';
                    int score = minimax(board, depth - 1, alpha, beta, false);
                    board[i][j] = ' '; // Undo the move

                    // Update the best score if necessary
                    bestScore = MAX(bestScore, score);

                    // Update the alpha value
                    alpha = MAX(alpha, bestScore);

                    // Prune the remaining branches if possible
                    if (beta <= alpha)
                    {
                        break;
                    }
                }
            }
        }

        return bestScore;
    }
    else
    {
        // The human player is the minimizing player
        int bestScore = INT_MAX; // Initialize the best score to the highest possible value

        // Generate a list of possible moves for the human player
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                if (board[i][j] == ' ' && is_valid_move(i, j))
                {
                    // Make the move and recursively search the resulting board state
                    board[i][j] = 'R';
                    int score = minimax(board, depth - 1, alpha, beta, true);
                    board[i][j] = ' '; // Undo the move

                    // Update the best score if necessary
                    bestScore = MIN(bestScore, score);

                    // Update the beta value
                    beta = MIN(beta, bestScore);

                    // Prune the remaining branches if possible
                    if (beta <= alpha)
                    {
                        break;
                    }
                }
            }
        }

        return bestScore;
    }
}

// Evaluate the board state and return a score
int evaluate_board()
{
    // Calculate the number of possible moves for each player
    int p1_moves = 0, p2_moves = 0;
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (board[i][j] == ' ' && is_valid_move(i, j))
            {
                if (player == 1)
                {
                    p1_moves++;
                }
                else
                {
                    p2_moves++;
                }
            }
        }
    }

    // Return a score based on the number of possible moves for each player
    if (p1_moves == 0 && p2_moves == 0)
    {
        // The game is a draw
        return 0;
    }
    else if (p1_moves == 0)
    {
        // The human player wins
        return -10;
    }
    else if (p2_moves == 0)
    {
        // The AI player wins
        return 10;
    }
    else
    {
        // The game is not over yet
        return (p1_moves - p2_moves);
    }
}
// Determine the best tile to remove using the minimax algorithm with alpha-beta pruning
void ai_remove_tile()
{
    int bestScore = INT_MIN;        // Initialize the best score to the lowest possible value
    int bestRow = -1, bestCol = -1; // Initialize the best tile to remove to an invalid position

    // Generate a list of possible tiles to remove
    for (int i = p2_x - 1; i <= p2_x + 1; i++)
    {
        for (int j = p2_y - 1; j <= p2_y + 1; j++)
        {
            if (board[i][j] == ' ')
            {
                // Remove the tile and search the resulting board state
                board[i][j] = 'X';
                int score = minimax(board, 4, INT_MIN, INT_MAX, false);
                board[i][j] = ' '; // Undo the remove

                // Update the best score and tile to remove if necessary
                if (score > bestScore)
                {
                    bestScore = score;
                    bestRow = i;
                    bestCol = j;
                }
            }
        }
    }

    // Remove the best tile
    if (bestRow != -1 && bestCol != -1)
    {
        board[bestRow][bestCol] = 'X';
    }
}

// Write the current player's move to the shared file
void write_move(int move_x, int move_y, int remove_x, int remove_y)
{
    // Open the shared file in append mode
    FILE *file = fopen("shared_file.txt", "a");

    // Write the current turn number, player id, and move coordinates to the file
    fprintf(file, "%d;%d;%c;%d;%c;%d;\n", turn_id, player, 'a' + move_y, move_x + 1, 'a' + remove_y, remove_x + 1);

    // Close the file
    fclose(file);
}

// Read the opponent player's move from the shared file
void read_move(int *move_x, int *move_y, int *remove_x, int *remove_y)
{
    // Open the shared file in read mode
    FILE *file = fopen("shared_file.txt", "r");

    // Read the file line by line until the correct turn is found
    char line[100];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Parse the turn id and player id from the line
        int turn, player;
        sscanf(line, "%d;%d;", &turn, &player);

        // Check if the line corresponds to the current turn and the opponent player
        if (turn == turn_id)
        {
            // Parse the move and remove coordinates from the line
            char move_y_char, remove_y_char;
            sscanf(line, "%*d;%*d;%c;%d;%c;%d;", &move_y_char, move_x, &remove_y_char, remove_x);

            // Convert the move and remove coordinates to integers
            *move_y = move_y_char - 'a';
            *move_x -= 1;
            *remove_y = remove_y_char - 'a';
            *remove_x -= 1;

            // Close the file and return
            fclose(file);
            return;
        }
    }

    // Close the file
    fclose(file);
}
int char_to_index(char c)
{
    if (c >= 'a' && c <= 'g')
    {
        return c - 'a';
    }
    else
    {
        // Return -1 for invalid characters
        return -1;
    }
}