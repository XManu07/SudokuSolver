#include "SudokuSolver.h"

std::vector<int> SudokuSolver::PuzzleStringToVector(const std::string& sudokuString)
{
    std::vector<int> puzzle(81, 0);
    int i = 0;

    for (char c : sudokuString) {
        if (i >= 81)
            break;

        if (c >= '0' && c <= '9') {
            puzzle[i++] = c - '0';
        }
    }

    return puzzle;
}

std::string SudokuSolver::PuzzleVectorToString(const std::vector<int>& puzzle)
{
    std::string result;
    for (unsigned int i = 0; i < puzzle.size(); i++) {
        result += std::to_string(puzzle[i]);
    }
    return result;
}

bool SudokuSolver::IsValidPuzzle(const std::vector<int>& puzzle)
{
    if (puzzle.size() != 81)
        return false;

    for (int digit : puzzle) {
        if (digit < 0 || digit>9) {
            return false;
        }
    }

    // check for duplicated number on row
    for (int row = 0; row < 9; row++) {
        std::vector<bool> nrUsed(10, false);
        for (int col = 0; col < 9; col++) {
            int digit = puzzle[row * 9 + col];
            if (digit != 0) {
                if (nrUsed[digit]) {
                    return false;
                }
                nrUsed[digit] = true;
            }
        }
    }

    // check for duplicated number on column
    for (int col = 0; col < 9; col++) {
        std::vector<bool> nrUsed(10, false);
        for (int row = 0; row < 9; row++) {
            int digit = puzzle[row * 9 + col];
            if (digit != 0) {
                if (nrUsed[digit]) {
                    return false;
                }
                nrUsed[digit] = true;
            }
        }
    }

    // check for duplicated number on box
    for (int bx = 0; bx < 3; bx++) {
        for (int by = 0; by < 3; by++) {
            std::vector<bool> nrUsed(10, false);
            for (int row = by * 3; row < by * 3 + 3; row++) {
                for (int col = bx * 3; col < bx * 3 + 3; col++) {
                    int digit = puzzle[row * 9 + col];
                    if (digit != 0) {
                        if (nrUsed[digit]) {
                            return false;
                        }
                        nrUsed[digit] = true;
                    }
                }
            }
        }
    }

    return true;
}

void SudokuSolver::InitializeStructure()
{
    dlx.SetupSudokuStructure();
}

void SudokuSolver::DeleteStructure()
{
    dlx.DeleteSudokuStructure();
}

bool SudokuSolver::Solve(const std::string& sudokuString)
{
    std::vector<int> sudokuVector = PuzzleStringToVector(sudokuString);

    dlx.ResetStructure();

    dlx.CoverExistingConstraints(sudokuVector);

    return dlx.Solve();
}

std::vector<int> SudokuSolver::GetSolution()
{
    return dlx.GetSolution();
}


