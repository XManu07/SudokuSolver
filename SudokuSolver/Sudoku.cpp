#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include "Node.h"
#include "ColumnHeader.h"
#include "SudokuSolver.h"

void processPuzzles(size_t startIndex, size_t puzzlesNumber,
    std::vector<std::string>& sudokuPuzzles) {

    SudokuSolver solver;
    solver.InitializeStructure();

    size_t puzzleProcessedCount = 0;
    for (size_t i = startIndex; i < startIndex + puzzlesNumber; i++) {
        bool solved = solver.Solve(sudokuPuzzles[i]);
        std::string resultString = solved ?
            solver.PuzzleVectorToString(solver.GetSolution()) : "unsolved";

        {
            sudokuPuzzles[i] = resultString;
        }
        puzzleProcessedCount++;
    }

    solver.DeleteStructure();
}

int main() {
    std::ifstream inputFile("input.txt");
    std::ofstream outputFile("output.txt");
    std::vector<std::string> sudokuPuzzles;

    unsigned int threadsNumber = std::thread::hardware_concurrency();
	std::vector<std::thread> threads(threadsNumber);
    auto startTime = std::chrono::high_resolution_clock::now();


    size_t puzzlesCount = 0;
    std::string line;
    while (std::getline(inputFile, line) && puzzlesCount<100000) {
        sudokuPuzzles.push_back(line);
        puzzlesCount++;
    }
    inputFile.close();

    size_t puzzlesPerThread = sudokuPuzzles.size() / threadsNumber;
    size_t remainingPuzzles = sudokuPuzzles.size() % threadsNumber;

    size_t startIndex = 0;

    startIndex = 0;
    for (size_t i = 0; i < threadsNumber; i++) {
        size_t linesToProcess = puzzlesPerThread;
        if (i < remainingPuzzles) {
            linesToProcess++;
        }
		threads[i] = std::thread(processPuzzles,
                                startIndex,
                                linesToProcess,
			                    std::ref(sudokuPuzzles));
        startIndex += linesToProcess;
    }

	for (auto& thread : threads) {
        thread.join();
	}

    for (const auto& result : sudokuPuzzles) {
        outputFile << result << std::endl;
    }
    outputFile.close();

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> solvingTime = endTime - startTime;

    std::cout   << " | Puzzles/sec: " << std::fixed << std::setprecision(2)
                << (puzzlesCount / solvingTime.count()) << std::endl;

    return 0;
}