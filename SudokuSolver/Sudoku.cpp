#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <atomic>
#include <mutex>
#include <future>
#include "Node.h"
#include "ColumnHeader.h"
#include "SudokuSolver.h"

void processPuzzles(const std::string& inputFilename,
	size_t startLine, size_t numLines,
    std::vector<std::string>& sudokuPuzzles,
    std::mutex& mtx) {

    SudokuSolver solver;

    std::ifstream inputFile(inputFilename);
    std::string line;

    for (size_t i = 0; i < startLine; i++) {
        if (!std::getline(inputFile, line)) {
            std::cout << "Thread couldn't seek to starting line " << startLine << std::endl;
        }
    }

    size_t puzzleProcessedCount = 0;
    while (puzzleProcessedCount < numLines && std::getline(inputFile, line)) {
        bool solved = solver.Solve(line);
        if (solved) {
            std::string solutionString = solver.PuzzleVectorToString(solver.GetSolution());
            {
                std::lock_guard<std::mutex> lock(mtx);
                sudokuPuzzles[startLine + puzzleProcessedCount] = solutionString;
            }
        }
        else {
            std::lock_guard<std::mutex> lock(mtx);
            sudokuPuzzles[startLine + puzzleProcessedCount] = "unsolved";
        }
        puzzleProcessedCount++;
    }

    inputFile.close();
}

int main() {
    std::ifstream inputFile("input.txt");
    std::ofstream outputFile("output.txt");
    std::vector<std::string> sudokuPuzzles;
    std::string line;
    std::mutex mutex;

    unsigned int threadsNumber = std::thread::hardware_concurrency();
	std::vector<std::thread> threads(threadsNumber);
    auto startTime = std::chrono::high_resolution_clock::now();

    size_t puzzlesCount = 0;
    while (std::getline(inputFile, line) && puzzlesCount < 100000) {
        sudokuPuzzles.push_back(line);
        puzzlesCount++;
    }
    inputFile.close();

    size_t puzzlesPerThread = sudokuPuzzles.size() / threadsNumber;
    size_t remainingPuzzles = sudokuPuzzles.size() % threadsNumber;

    size_t startLine = 0;

    startLine = 0;
    for (size_t i = 0; i < threadsNumber; i++) {
        size_t linesToProcess = puzzlesPerThread;
        if (i < remainingPuzzles) {
            linesToProcess++;
        }
		threads[i] = std::thread(processPuzzles,
                                "input.txt",
                                startLine,
                                linesToProcess,
			                    std::ref(sudokuPuzzles),
                                std::ref(mutex));
        startLine += linesToProcess;
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

    std::cout << "Threads: " << threadsNumber
        << " | Puzzles: " << puzzlesCount
        << " | Wall time: " << std::fixed << std::setprecision(3)
        << solvingTime.count() << "s"
        << " | Puzzles/sec: " << std::fixed << std::setprecision(2)
        << (puzzlesCount / solvingTime.count()) << std::endl;

    return 0;
}

//#include <thread>
//#include <iostream>
//#include <fstream>
//#include <string>
//#include <vector>
//#include <chrono>
//#include <iomanip>
//#include <atomic>
//#include <mutex>
//#include <future>
//#include "Node.h"
//#include "ColumnHeader.h"
//#include "SudokuSolver.h"
//
//std::vector<std::string> processPuzzles(const std::string& inputFilename,
//    size_t startLine, size_t numLines) {
//
//    std::vector<std::string> threadResults(numLines, "UNSOLVED");
//
//    SudokuSolver solver;
//
//    std::ifstream inputFile(inputFilename);
//    std::string line;
//
//    for (size_t i = 0; i < startLine; i++) {
//        if (!std::getline(inputFile, line)) {
//            std::cout << "Thread couldn't seek to starting line " << startLine << std::endl;
//            return threadResults;
//        }
//    }
//
//    size_t puzzleProcessedCount = 0;
//    while (puzzleProcessedCount < numLines && std::getline(inputFile, line)) {
//        bool solved = solver.Solve(line);
//        if (solved) {
//            std::string solutionString = solver.PuzzleVectorToString(solver.GetSolution());
//            threadResults[puzzleProcessedCount] = solutionString;
//        }
//        puzzleProcessedCount++;
//    }
//
//    inputFile.close();
//    return threadResults;
//}
//
//int main() {
//    std::ifstream inputFile("input.txt");
//    std::ofstream outputFile("output.txt");
//    std::vector<std::string> sudokuPuzzles;
//    std::string line;
//
//    unsigned int threadsNumber = std::thread::hardware_concurrency();
//    auto startTime = std::chrono::high_resolution_clock::now();
//
//    size_t puzzlesCount = 0;
//    while (std::getline(inputFile, line) && puzzlesCount < 100000) {
//        sudokuPuzzles.push_back(line);
//        puzzlesCount++;
//    }
//    inputFile.close();
//
//    size_t puzzlesPerThread = sudokuPuzzles.size() / threadsNumber;
//    size_t remainingPuzzles = sudokuPuzzles.size() % threadsNumber;
//
//    std::vector<std::future<std::vector<std::string>>> futures;
//
//    size_t startLine = 0;
//    for (size_t i = 0; i < threadsNumber; ++i) {
//        size_t linesToProcess = puzzlesPerThread;
//        if (i < remainingPuzzles) {
//            linesToProcess++;
//        }
//
//        if (linesToProcess > 0) {
//            futures.push_back(std::async(std::launch::async, processPuzzles,
//                "input.txt", startLine, linesToProcess));
//        }
//
//        startLine += linesToProcess;
//    }
//
//    startLine = 0;
//    for (size_t i = 0; i < futures.size(); ++i) {
//        size_t linesToProcess = puzzlesPerThread;
//        if (i < remainingPuzzles) {
//            linesToProcess++;
//        }
//
//        std::vector<std::string> threadResults = futures[i].get();
//
//        for (size_t j = 0; j < threadResults.size(); ++j) {
//            sudokuPuzzles[startLine + j] = threadResults[j];
//        }
//
//        startLine += linesToProcess;
//    }
//
//    for (const auto& result : sudokuPuzzles) {
//        outputFile << result << std::endl;
//    }
//    outputFile.close();
//
//    auto endTime = std::chrono::high_resolution_clock::now();
//    std::chrono::duration<double> solvingTime = endTime - startTime;
//
//    std::cout << "Threads: " << threadsNumber
//        << " | Puzzles: " << puzzlesCount
//        << " | Wall time: " << std::fixed << std::setprecision(3)
//        << solvingTime.count() << "s"
//        << " | Puzzles/sec: " << std::fixed << std::setprecision(2)
//        << (puzzlesCount / solvingTime.count()) << std::endl;
//
//    return 0;
//}