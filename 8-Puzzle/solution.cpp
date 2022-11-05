#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string.h>
#include <chrono>
#include <iomanip>
using namespace std;
using namespace std::chrono;

int directions[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
string directionsDictionary[4] = { "down", "up", "right", "left" };

int n, freeTileIndex, puzzleSize;

struct Table
{
    vector<vector<int>> table;
    int h = 0;
    int x, y;
    string moveTo;

    bool operator==(const Table& t) const
    {
        for (int i = 0; i < puzzleSize; i++)
            for (int j = 0; j < puzzleSize; j++)
                if (table[i][j] != t.table[i][j])
                    return false;
        return true;
    }

    void calculateHeuristic()
    {
        int realRow, realCol;
        this->h = 0;
        for (int i = 0; i < puzzleSize; i++)
            for (int j = 0; j < puzzleSize; j++) {
                int num = table[i][j];
                if (num != 0) {
                    if (num < freeTileIndex) {
                        num--;
                    }

                    realRow = num / puzzleSize;
                    realCol = num % puzzleSize;
                }
                else {
                    realRow = freeTileIndex / puzzleSize;
                    realCol = freeTileIndex % puzzleSize;
                }

                this->h += abs(i - realRow) + abs(j - realCol);
            }
    }
};

int calculateHeuristicForPosition(const int& row, const int& column, int num)
{
    int realRow, realCol;
    if (num != 0) {
        if (num < freeTileIndex) {
            num--;
        }

        realRow = num / puzzleSize;
        realCol = num % puzzleSize;
    }
    else {
        realRow = freeTileIndex / puzzleSize;
        realCol = freeTileIndex % puzzleSize;
    }
    return abs(row - realRow) + abs(column - realCol);
}

Table startState, finalState;

bool hasSolution(const Table& t) {
    int inversions = 0, index = 0;
    vector<int> flattened;
    for (int i = 0; i < puzzleSize; i++)
        for (int j = 0; j < puzzleSize; j++)
            flattened.push_back(t.table[i][j]);

    for (int i = 0; i < puzzleSize; i++)
        for (int j = i + 1; j < puzzleSize; j++)
            if (flattened[i] && flattened[j] && flattened[i] > flattened[j])
                inversions++;

    return inversions % 2 == 0;
}

void input()
{
    scanf_s("%d", &n);
    scanf_s("%d", &freeTileIndex);
    puzzleSize = (int)sqrt(n + 1);
    freeTileIndex = freeTileIndex == -1 ? n : freeTileIndex;

    for (int i = 0; i < puzzleSize; i++) {
        vector<int> row;
        for (int j = 0; j < puzzleSize; j++)
        {
            int num;
            scanf_s("%d", &num);
            row.push_back(num);
            if (num == 0)
            {
                startState.x = i;
                startState.y = j;
            }
        }
        startState.table.push_back(row);
    }

    int cnt = 1;
    int freeTileX = freeTileIndex / puzzleSize, freeTileY = freeTileIndex % puzzleSize;
    for (int i = 0; i < puzzleSize; i++) {
        vector<int> row;
        for (int j = 0; j < puzzleSize; j++) {
            int num = (i == freeTileX && j == freeTileY) ? 0 : cnt++;
            row.push_back(num);
        }
        finalState.table.push_back(row);
    }

    finalState.h = 0;
    finalState.x = freeTileIndex / puzzleSize;
    finalState.y = freeTileIndex % puzzleSize;
    if (!hasSolution(finalState)) {
        cout << "No solution!" << endl;
        exit(1);
    }
    startState.calculateHeuristic();
}

int aStar(vector<Table>& path, int g, int bound)
{
    Table current = path.back();
    int f = g + current.h;
    if (f > bound) return f;
    if (current == finalState) return 0;
    int min = INT_MAX;

    //generate children
    Table child = current;
    int t;
    for (int i = 0; i < 4; i++) {
        int newX = current.x + directions[i][0];
        int newY = current.y + directions[i][1];

        if (newX < 0 || newX >= puzzleSize ||
            newY < 0 || newY >= puzzleSize)
            continue;

        child.h = current.h -
            calculateHeuristicForPosition(newX, newY, current.table[newX][newY]) +
            calculateHeuristicForPosition(current.x, current.y, current.table[newX][newY]) -
            calculateHeuristicForPosition(current.x, current.y, current.table[current.x][current.y]) +
            calculateHeuristicForPosition(newX, newY, current.table[current.x][current.y]);

        swap(child.table[current.x][current.y], child.table[newX][newY]);
        if (std::find(path.begin(), path.end(), child) != path.end()) {
            swap(child.table[current.x][current.y], child.table[newX][newY]);
            continue;
        }
        child.x = newX;
        child.y = newY;
        child.moveTo = directionsDictionary[i];

        path.push_back(child);
        t = aStar(path, g + 1, bound);
        if (t == 0) return 0;
        if (t < min) min = t;
        swap(child.table[current.x][current.y], child.table[newX][newY]);
        path.pop_back();
    }
    return min;
}

void printPath(vector<Table>& path) {
    cout << path.size() - 1 << endl;
    for (int i = 1; i < path.size(); i++) {
        cout << path[i].moveTo << endl;
    }
}

void iterativeDeepeningAStar() {
    int bound = startState.h;
    vector<Table> path;
    path.push_back(startState);
    int t;
    auto start = high_resolution_clock::now();
    while (true) {
        t = aStar(path, 0, bound);
        if (t == 0) break;
        bound = t;
    }
    auto stop = high_resolution_clock::now();
    printPath(path);
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << fixed;
    cout << setprecision(2);
    cout << "Time taken: " << (float)duration.count() / 1000 << "s" << endl;
}

int main()
{
    input();
    iterativeDeepeningAStar();

    return 0;
}

/**
Some tests:

8
-1
1 2 3
4 5 6
0 7 8

8
-1
1 0 3
4 2 5
7 8 6

8
-1
4 0 3
5 2 1
7 8 6

8
-1
0 8 6
5 4 3
2 1 7

15
-1
8 11 1 12
5 0 15 9
13 14 4 10
7 3 6 2
**/
