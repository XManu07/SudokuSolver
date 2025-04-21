#pragma once
#include <iostream>
#include <cstdint>
#include "DancingLinks.h"

class SudokuSolver
{
private:
	DancingLinks dlx;

public:
	std::vector<int> PuzzleStringToVector(const std::string& str);
	
	std::string PuzzleVectorToString(const std::vector<int>& puzzle);
	
	bool IsValidPuzzle(const std::vector<int>& puzzle);
	
	bool Solve(const std::string& sudokuString);

	std::vector<int> GetSolution();
};

