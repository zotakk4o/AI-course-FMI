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

vector<vector<int>> board;
vector<pair<int, int>>queens;
vector<pair<int, int>>queensConflict;
random_device rd; // obtain a random number from hardware
mt19937 generator; // seed the generator
int n, conflictingQueens;
vector<int> possibleMinimas;
uniform_int_distribution<> distr;

void printSolution(const vector<vector<int>>& queens) {
    int size = queens.size();
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            char symbol = queens[i][j] > 0 ? '_' : '*';
            cout << symbol << " ";
        }
        cout << "\n";
    }
}

bool queensAttackCol(const vector<pair<int, int>>queens, const pair<int, int>& queen) {
    for (const pair<int, int>& other : queens)
    {
        if (queen != other && queen.second == other.second) return true;
    }
    return false;
}

bool queensAttackRow(const vector<pair<int, int>>queens, const pair<int, int>& queen) {
    for (const pair<int, int>& other : queens)
    {
        if (queen != other && queen.first == other.first) return true;
    }
    return false;
}

bool queensAttackDiag(const vector<pair<int, int>>queens, const pair<int, int>& queen) {
    for (const pair<int, int>& other : queens)
    {
        if (queen != other && abs(queen.first - other.first) == abs(queen.second - other.second)) return true;
    }
    return false;
}

int unsafeQueens() {
    int res = 0;
    for (int i = 0; i < queens.size(); i++) {
        if (queensAttackCol(queens, queens[i]) || queensAttackRow(queens, queens[i]) || queensAttackDiag(queens, queens[i])) {
            res++;
            queensConflict.push_back(queens[i]);
        }
    }

    return res / 2;
}

bool areQueensSafe() {
    return unsafeQueens() == 0;
}

void printTable() {
    int size = board.size();
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            cout << board[i][j] << " ";
        }
        cout << "\n";
    }
}

int minConflictVal(const int& col) {
    int size = board.size();
    int minConflicts = INT_MAX;
    for (int row = size - 1; row >= 0; row--)
    {
        if (board[row][col] < minConflicts) {
            minConflicts = board[row][col];
        }
    }

    return minConflicts;
}

void updateConflicts(const int& row, const int& col, const bool& increase = true, const bool& isInit = false) {
    int size = board.size();

    for (int left = col - 1; left >= 0; left--)
    {
        if (increase) {
            board[row][left]++;
            if (!isInit && board[row][left] - 1 == 0) {
                pair<int, int> queen = { row, left };
                queensConflict.push_back(queen);
            }
        }
        else {
            board[row][left]--;
            if (board[row][left] == 0) {
                pair<int, int> queen = { row, left };
                auto search = find(queensConflict.begin(), queensConflict.end(), queen);
                if (search != queensConflict.end())
                    queensConflict.erase(search);
            }
        }
    }

    for (int right = col + 1; right < size; right++)
    {
        if (increase) {
            board[row][right]++;
            if (!isInit && board[row][right] - 1 == 0) {
                pair<int, int> queen = { row, right };
                queensConflict.push_back(queen);
            }
        }
        else {
            board[row][right]--;
            if (board[row][right] == 0) {
                pair<int, int> queen = { row, right };
                auto search = find(queensConflict.begin(), queensConflict.end(), queen);
                if (search != queensConflict.end())
                    queensConflict.erase(search);
            }
        }
    }

    for (int up = row + 1; up < size; up++)
    {
        if (increase) {
            board[up][col]++;
            if (!isInit && board[up][col] - 1 == 0) {
                pair<int, int> queen = { up, col };
                queensConflict.push_back(queen);
            }
        }
        else {
            board[up][col]--;
            if (board[up][col] == 0) {
                pair<int, int> queen = { up, col };
                auto search = find(queensConflict.begin(), queensConflict.end(), queen);
                if (search != queensConflict.end())
                    queensConflict.erase(search);
            }
        }
    }

    for (int down = row - 1; down >= 0; down--)
    {
        if (increase) {
            board[down][col]++;
            if (!isInit && board[down][col] - 1 == 0) {
                pair<int, int> queen = { down, col };
                queensConflict.push_back(queen);
            }
        }
        else {
            board[down][col]--;
            if (board[down][col] == 0) {
                pair<int, int> queen = { down, col };
                auto search = find(queensConflict.begin(), queensConflict.end(), queen);
                if (search != queensConflict.end())
                    queensConflict.erase(search);
            }
        }
    }

    for (int i = 1; row - i >= 0 && col + i < size; i++) {
        if (increase) {
            board[row - i][col + i]++;
            if (!isInit && board[row - i][col + i] - 1 == 0) {
                pair<int, int> queen = { row - i, col + i };
                queensConflict.push_back(queen);
            }
        }
        else {
            board[row - i][col + i]--;
            if (board[row - i][col + i] == 0) {
                pair<int, int> queen = { row - i, col + i };
                auto search = find(queensConflict.begin(), queensConflict.end(), queen);
                if (search != queensConflict.end())
                    queensConflict.erase(search);
            }
        }
    }

    for (int i = 1; row + i < size && col + i < size; i++) {
        if (increase) {
            board[row + i][col + i]++;
            if (!isInit && board[row + i][col + i] - 1 == 0) {
                pair<int, int> queen = { row + i, col + i };
                queensConflict.push_back(queen);
            }
        }
        else {
            board[row + i][col + i]--;
            if (board[row + i][col + i] == 0) {
                pair<int, int> queen = { row + i, col + i };
                auto search = find(queensConflict.begin(), queensConflict.end(), queen);
                if (search != queensConflict.end())
                    queensConflict.erase(search);
            }
        }
    }

    for (int i = 1; row - i >= 0 && col - i >= 0; i++) {
        if (increase) {
            board[row - i][col - i]++;
            if (!isInit && board[row - i][col - i] - 1 == 0) {
                pair<int, int> queen = { row - i, col - i };
                queensConflict.push_back(queen);
            }
        }
        else {
            board[row - i][col - i]--;
            if (board[row - i][col - i] == 0) {
                pair<int, int> queen = { row - i, col - i };
                auto search = find(queensConflict.begin(), queensConflict.end(), queen);
                if (search != queensConflict.end())
                    queensConflict.erase(search);
            }
        }
    }

    for (int i = 1; row + i < size && col - i >= 0; i++) {
        if (increase) {
            board[row + i][col - i]++;
            if (!isInit && board[row + i][col - i] - 1 == 0) {
                pair<int, int> queen = { row + i, col - i };
                queensConflict.push_back(queen);
            }
        }
        else {
            board[row + i][col - i]--;
            if (board[row + i][col - i] == 0) {
                pair<int, int> queen = { row + i, col - i };
                auto search = find(queensConflict.begin(), queensConflict.end(), queen);
                if (search != queensConflict.end())
                    queensConflict.erase(search);
            }
        }
    }
}

bool iterativeMinConflicts(int iterations) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            board[i][j] = 0;
        }
    }

    queensConflict.clear();

    //place queens by MinConflicts
    for (int col = 0; col < n; col++)
    {
        int minConfval = minConflictVal(col);
        int possiblePositions = -1;
        for (int row = 0; row < n; row++) {
            if (board[row][col] == minConfval) {
                possibleMinimas[++possiblePositions] = row;
            }
        }

        uniform_int_distribution<> distrMin(0, possiblePositions);
        int row = possibleMinimas[distrMin(generator)];
        queens[col] = { row, col };
        updateConflicts(row, col, true, true);
    }

    unsafeQueens();
    int retriesBeforeRandomMove = 10;
    while (!queensConflict.empty() && --iterations) {
        int minAttacks = n + 1;
        //int queenPos = distr(generator);
        uniform_int_distribution<> queenGen(0, queensConflict.size() - 1);
        int queenPos = queenGen(generator);
        pair<int, int> queen = queensConflict[queenPos];
        if (!board[queen.first][queen.second]) {
            queensConflict.erase(queensConflict.begin() + queenPos);
            continue;
        }
        int minRow = queen.first;
        //find min conflicts position
        for (int row = 0; row < n; row++) {
            if (board[row][queen.second] < minAttacks) {
                minAttacks = board[row][queen.second];
            }
        }

        int possiblePositions = -1;
        for (int row = 0; row < n; row++) {
            if (board[row][queen.second] == minAttacks && row != queen.first) {
                possibleMinimas[++possiblePositions] = row;
            }
        }

        if (possiblePositions == -1) {
            if (--retriesBeforeRandomMove) {
                continue;
            }
            minRow = distr(generator);
            retriesBeforeRandomMove = 20;
        }
        else {
            uniform_int_distribution<> distrMin(0, possiblePositions);
            minRow = possibleMinimas[distrMin(generator)];
        }

        //move queen to minConflictPosition
        if (--board[minRow][queen.second] == 0) {
            queensConflict.erase(queensConflict.begin() + queenPos);//move to non-conflict spot
        }
        else {
            queensConflict[queenPos] = { minRow, queen.second };//remain in conflict spot
        }
        updateConflicts(minRow, queen.second);
        updateConflicts(queen.first, queen.second, false);
    }

    return queensConflict.empty();
}

int main()
{
    scanf_s("%d", &n);
    for (int i = 0; i < n; i++) {
        board.push_back(vector<int>(n));
    }
    int iterations = n;
    possibleMinimas = vector<int>(n);
    distr = uniform_int_distribution<>(0, n - 1);
    queens = vector<pair<int, int>>(n);
    generator = mt19937(rd());

    auto start = high_resolution_clock::now();
    while (!iterativeMinConflicts(iterations));
    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(stop - start);
    cout << fixed;
    cout << setprecision(2);
    cout << "Time taken: " << (float)duration.count() / 1000 << "s" << endl;
    if (n <= 20) {
        printSolution(board);
    }
    return 0;
}
