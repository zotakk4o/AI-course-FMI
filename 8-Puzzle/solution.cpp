#include <cstdio>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string.h>
#include <stack>
using namespace std;

int directions[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
string directionsDictionary[4] = { "down", "up", "right", "left" };

int n, freeTileIndex, puzzleSize;

struct Table
{
    vector<vector<int>> table;
    int h = 0;
    int g = 0;
    int x, y;
    string moveTo;

    int f() const
    {
        return g + h;
    }

    friend bool operator< (const Table& t1, const Table& t2)
    {
        return t1.f() < t2.f();
    }

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

vector<string> path;

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

bool tableAlreadyPassed(const Table& t, const vector<Table>& passedTables)
{
    int sz = passedTables.size();
    for (int i = 0; i < sz; i++)
        if (t == passedTables[i])
            return true;
    return false;
}

void aStar(Table& current, Table& goal, int limit)
{
    if (current == goal) {
        cout << path.size() << endl;
        for (int i = 0; i < path.size(); i++) {
            cout << path[i] << endl;
        }
        exit(0);
    }

    if (limit == 0) {
        return;
    }
    
    priority_queue<Table> pq;
    Table child = current;
    child.g = current.g + 1;
    //generate children
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
        child.x = newX;
        child.y = newY;
        child.moveTo = directionsDictionary[i];
        pq.push(child);
        swap(child.table[current.x][current.y], child.table[newX][newY]);
    }

    while (!pq.empty()) {
        Table currTable = pq.top();
        pq.pop();
        //path.push_back(currTable.moveTo);
        aStar(currTable, goal, limit - 1);
        //path.pop_back();
    }
}

void iterativeDeepeningAStar(int limit) {
    for (int i = 1; i <= limit; i++) {
        path.clear();
        cout << i << endl;
        aStar(startState, finalState, i);
    }
}

int main()
{
    input();
    iterativeDeepeningAStar(50);

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
4 2 3
5 0 1
7 8 6
**/
