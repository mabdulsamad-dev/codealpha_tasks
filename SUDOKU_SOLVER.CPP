#include <iostream>
#include <sstream>
#include <string>

using namespace std;

const int N = 9;

// Print the 9x9 Sudoku grid cleanly
void printGrid(const int grid[N][N]) {
    cout << "\n+-------+-------+-------+\n";
    for (int row = 0; row < N; row++) {
        cout << "| ";
        for (int col = 0; col < N; col++) {
            if (grid[row][col] == 0)
                cout << ". ";
            else
                cout << grid[row][col] << " ";

            if ((col + 1) % 3 == 0)
                cout << "| ";
        }
        cout << "\n";
        if ((row + 1) % 3 == 0)
            cout << "+-------+-------+-------+\n";
    }
}

// Check whether placing num at grid[row][col] is valid
bool isSafe(const int grid[N][N], int row, int col, int num) {
    // Check row and column
    for (int x = 0; x < N; x++) {
        if (grid[row][x] == num || grid[x][col] == num)
            return false;
    }

    // Check 3x3 subgrid
    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid[i + startRow][j + startCol] == num)
                return false;
        }
    }

    return true;
}

// Recursive backtracking solver
bool solveSudoku(int grid[N][N]) {
    int row = -1;
    int col = -1;
    bool isEmpty = false;

    // Find next unassigned cell (represented by 0)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (grid[i][j] == 0) {
                row = i;
                col = j;
                isEmpty = true;
                break;
            }
        }
        if (isEmpty)
            break;
    }

    // Base case: no empty cells left
    if (!isEmpty)
        return true;

    // Try digits 1 through 9
    for (int num = 1; num <= 9; num++) {
        if (isSafe(grid, row, col, num)) {
            grid[row][col] = num;

            if (solveSudoku(grid))
                return true;

            // Backtrack
            grid[row][col] = 0;
        }
    }

    return false;
}

// Validate that the initial user board has no conflicting numbers
bool isValidInitialBoard(int grid[N][N]) {
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            int val = grid[r][c];
            if (val < 0 || val > 9)
                return false;
            if (val != 0) {
                grid[r][c] = 0;
                bool ok = isSafe(grid, r, c, val);
                grid[r][c] = val;
                if (!ok)
                    return false;
            }
        }
    }
    return true;
}

int main() {
    int sampleGrid[N][N] = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };

    cout << "==============================================\n";
    cout << "        CODEALPHA - C++ SUDOKU SOLVER\n";
    cout << "==============================================\n";
    cout << "1. Solve Built-in Sample Puzzle\n";
    cout << "2. Enter Custom 9x9 Puzzle (use 0 for blanks)\n";
    cout << "----------------------------------------------\n";
    cout << "Select an option (1-2): ";

    string input;
    getline(cin, input);
    stringstream ss(input);
    int choice;

    if (!(ss >> choice) || (ss >> ws && !ss.eof()) || (choice != 1 && choice != 2)) {
        cout << "[Error] Invalid selection.\n";
        return 1;
    }

    int grid[N][N];

    if (choice == 1) {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                grid[i][j] = sampleGrid[i][j];
    } else {
        cout << "\nEnter 9 rows of 9 numbers separated by spaces (0 for empty cells):\n";
        for (int i = 0; i < N; i++) {
            cout << "Row " << (i + 1) << ": ";
            string rowLine;
            getline(cin, rowLine);
            stringstream rowStream(rowLine);

            for (int j = 0; j < N; j++) {
                if (!(rowStream >> grid[i][j]) || grid[i][j] < 0 || grid[i][j] > 9) {
                    cout << "[Error] Invalid row input. Enter exactly 9 digits (0-9).\n";
                    return 1;
                }
            }

            // Reject extra tokens or trailing characters on the row
            if (rowStream >> ws && !rowStream.eof()) {
                cout << "[Error] Too many values entered. Enter exactly 9 digits (0-9) per row.\n";
                return 1;
            }
        }
    }

    if (!isValidInitialBoard(grid)) {
        cout << "\n[Error] The initial Sudoku board contains conflicting digits.\n";
        return 1;
    }

    cout << "\nInitial Sudoku Board:";
    printGrid(grid);

    if (solveSudoku(grid)) {
        cout << "\n[Success] Solved Sudoku Board:";
        printGrid(grid);
    } else {
        cout << "\n[Error] No valid solution exists for this Sudoku board.\n";
    }

    return 0;
}
