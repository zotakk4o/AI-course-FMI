#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <utility>
#include <chrono>
#include <iomanip>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
using namespace std::chrono;
using namespace std;

int user;
int x = -1, y = -1;
int ourX = -1, ourY = -1;
const int pc = 1;
const int opponent = 2;

void printBoard(vector<vector<int>>& board) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == pc) {
                cout << 'X';
            }
            else if (board[i][j] == opponent) {
                cout << 'O';
            }
            else {
                cout << '_';
            }
            cout << ' ';
        }
        cout << "\n";
    }
}

void input() {
    cout << "Are you first or second in the game? ";
    scanf_s("%d", &user);

    if (user == 1) {
        cout << "What is your move? ";
        scanf_s("%d", &x);
        scanf_s("%d", &y);
    }
    else {
        ourX = rand() % 3;
        ourY = rand() % 3;
    }
}

bool areMovesLeft(vector<vector<int>>& board)
{
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == 0) {
                return true;
            }   
        }
    }
        
    return false;
}

int evaluate(vector<vector<int>>& board)
{
    for (int row = 0; row < 3; row++)
    {
        if (board[row][0] == board[row][1] && board[row][1] == board[row][2]) {
            if (board[row][0] == pc) {
                return 1;
            }  
            else if (board[row][0] == opponent) {
                return -1;
            }
                
        }
    }

    for (int col = 0; col < 3; col++)
    {
        if (board[0][col] == board[1][col] && board[1][col] == board[2][col])
        {
            if (board[0][col] == pc) {
                return 1;
            }  
            else if (board[0][col] == opponent) {
                return -1;
            }        
        }
    }

    if (board[0][0] == board[1][1] && board[1][1] == board[2][2])
    {
        if (board[0][0] == pc) {
            return 1;
        }      
        else if (board[0][0] == opponent) {
            return -1;
        }
    }

    if (board[0][2] == board[1][1] && board[1][1] == board[2][0])
    {
        if (board[0][2] == pc) {
            return 1;
        }
        else if (board[0][2] == opponent) {
            return -1;
        }
    }

    return 0;
}

void initializeBoard(vector<vector<int>>& board) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (x == i && y == j) {
                board[i][j] = opponent;
            }
            else if (ourX == i && ourY == j) {
                board[i][j] = pc;
            }
            else {
                board[i][j] = 0;
            }
        }
    }
}

pair<int, pair<int, int>> minimax_alpha_beta(vector<vector<int>>& board, int mover, int alpha, int beta)
{
    std::pair<int, int> bestMove = {-1, -1};
    const int score = evaluate(board);

    if (!areMovesLeft(board) || score != 0)
    {
        return { score, bestMove };
    }

    int bestScore = mover == pc ? INT_MIN : INT_MAX;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            pair<int, int> currMove = {i, j};

            if (board[i][j] != 0) {
                continue;
            }

            board[currMove.first][currMove.second] = mover;

            if (mover == pc) // Maximizing pc's turn
            {
                const int score = minimax_alpha_beta(board, opponent, alpha, beta).first;
                if (bestScore < score) {
                    bestScore = score;
                    bestMove = currMove;
                    board[currMove.first][currMove.second] = 0;
                    alpha = max(alpha, bestScore);
                    if (beta <= alpha)
                    {
                        break;
                    }
                }
            }
            else // Minimizing opponent's turn
            {
                const int score = minimax_alpha_beta(board, pc, alpha, beta).first;
                if (bestScore > score) {
                    bestScore = score;
                    bestMove = currMove;
                    board[currMove.first][currMove.second] = 0;
                    beta = min(beta, bestScore);
                    if (beta <= alpha)
                    {
                        break;
                    }
                }
            }

            board[currMove.first][currMove.second] = 0; // Undo move
        }
    }

    return { bestScore, bestMove };
}


int main()
{
    srand(time(NULL));
    input();
    pair<int, int> userMove{ x, y };
    vector<vector<int>> board;
    for (int i = 0; i < 3; i++)
    {
        board.push_back(vector<int>(3));
    }

    initializeBoard(board);

    const int alpha = INT_MIN;
    const int beta = INT_MAX;

    if (user == 1) {
        pair<int, pair<int, int>> ourFirstMove = minimax_alpha_beta(board, pc, alpha, beta);
        board[ourFirstMove.second.first][ourFirstMove.second.second] = pc;
    }

    printBoard(board);

    while (areMovesLeft(board) && evaluate(board) == 0) {
        cout << "What is your next move? ";

        scanf_s("%d", &x);
        scanf_s("%d", &y);

        if (board[x][y])
        {
            cout << "The position (" << x << ", " << y << ") is occupied. Try another one!" << endl;
            continue;
        }
        else
        {
            board[x][y] = opponent;
        }

        if (evaluate(board) != 0) {
            break;
        }

        pair<int, pair<int, int>> move = minimax_alpha_beta(board, pc, alpha, beta);
        board[move.second.first][move.second.second] = pc;

        printBoard(board);
    }

    const int score = evaluate(board);

    if (score == 0) {
        cout << "Tie!" << endl;
    }
    else {
        cout << "Winner is: " << (score == 1 ? "PC" : "you") << endl;
    }

    return 0;
}
