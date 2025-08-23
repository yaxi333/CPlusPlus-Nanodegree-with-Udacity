#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <windows.h>

using std::vector;

enum class State { kEmpty, kObstacle, kClosed, kPath, kStart, kFinish };

static std::string CellString(State state)
{
    switch (state) {
    case State::kObstacle: return "⛰️   ";
    case State::kPath: return "🚗   ";
    case State::kStart: return "🚦 ";
    case State::kFinish: return "🏁 ";
    default: return "0   ";
    }
}

vector<State> ParseLine(std::string line) {
    std::istringstream sline(line);
    std::string token;
    vector<State> values;
    int num;
    while (getline(sline, token, ',')) {
        num = std::stoi(token);
        if (num == 0)
            values.push_back(State::kEmpty);
        else
            values.push_back(State::kObstacle);
    }
    return std::move(values);
}

vector<vector<State>> ReadBoardFile(std::string path) {
    std::ifstream myfile(path);
    vector<vector<State>> board;
    if (myfile) {
        std::string line;
        while (getline(myfile, line)) {
            board.push_back(ParseLine(line));
            std::cout << line << "\n";
        }
    }
    return std::move(board);
}

bool CheckValidCell(int x, int y, vector<vector<State>>& grid)
{
    if (x < 0 or y < 0 or x >= grid.size() or y >= grid[0].size())    // or is the same as ||. Both can be used
        return false;
    if (grid[x][y] != State::kEmpty)
        return false;
    return true;
}

void AddToOpen(int x, int y, int g, int h, vector<vector<int>>& openNodes, vector<vector<State>>& grid)
{
    vector<int> newNode = { x,y,g,h };
    openNodes.push_back(newNode);
    grid[x][y] = State::kClosed;
}

bool Compare(const vector<int>& v1, const vector<int>& v2)
{
    int f1 = v1[2] + v1[3];
    int f2 = v2[2] + v2[3];

    return f1 > f2;
}

void CellSort(vector<vector<int>>* open)
{
    std::sort(open->begin(), open->end(), Compare);   // Notice: if we have sort in a function better to use pointer of vector as input, better performance and in-place sorting!
    // Then open->begin() not open.begin()
    // Q: The third one needs to be just a function do not need to give the input value? Why?
}

int Heuristic(int x1, int y1, int x2, int y2) {
    return abs(x2 - x1) + abs(y2 - y1);
}

void ExpandNeighbors(vector<int>& currentNode, vector<int>& goal, vector<vector<int>>& open, vector<vector<State>>& grid)
{
    const int delta[4][2] = { {-1, 0}, {0, -1}, {1, 0}, {0, 1} };
    int x, y, g, h;
    for (const auto& dir : delta) {
        x = dir[0] + currentNode[0];
        y = dir[1] + currentNode[1];
        if (x < 0 or y < 0 or x >= grid.size() or y >= grid[0].size())
            continue;
        if (grid[x][y] != State::kEmpty)
            continue;
        g = currentNode[2] + 1;
        h = Heuristic(x, y, goal[0], goal[1]);
        AddToOpen(x, y, g, h, open, grid);
    }
}

vector<vector<State>> Search(vector<vector<State>>& grid, vector<int>& init, vector<int>& goal)  // safer than using int init[2] as input
{
    vector<vector<int>> open{};
    int x, y;
    vector<int> currentNode;
    x = init[0];
    y = init[1];
    AddToOpen(x, y, 0, Heuristic(x, y, goal[0], goal[1]), open, grid);

    while (!open.empty()) {
        CellSort(&open);
        currentNode = open.back();
        open.pop_back();
        x = currentNode[0];
        y = currentNode[1];

        grid[x][y] = State::kPath;

        if (x == goal[0] && y == goal[1]) {
            // Should assign them before return because it may not succeed.
            grid[init[0]][init[1]] = State::kStart;
            grid[goal[0]][goal[1]] = State::kFinish;
            return grid;
        }

        ExpandNeighbors(currentNode, goal, open, grid);
    }

}

void PrintBoard(const vector<vector<State>> board) {
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[i].size(); j++) {
            SetConsoleOutputCP(CP_UTF8);    // To tells the Windows console to interpret output as UTF-8.
            std::cout << CellString(board[i][j]);
        }
        std::cout << "\n";
    }
}

int main() {
    vector<int> init = { 0, 0 };
    vector<int> goal = { 4, 5 };
    vector<vector<State>> board;

    board = ReadBoardFile("1.board");

    auto solution = Search(board, init, goal);

    PrintBoard(solution);

    std::cin.get();
}