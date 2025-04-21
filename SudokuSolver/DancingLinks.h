#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "ColumnHeader.h"
#include "Node.h"

class DancingLinks
{
private:
	// entry point for header
	ColumnHeader* header;
	std::unordered_map<std::string, ColumnHeader*> columnHeaders;
	std::vector<Node*> solution;
	std::vector<Node*> allNodes;
	bool solutionFound;\


public:
	DancingLinks();

	~DancingLinks();

	ColumnHeader* ChooseColumn();

	void CoverColumn(ColumnHeader* col);

	void UncoverColumn(ColumnHeader* col);

	bool SearchForSolution(int depth = 0);

	void DeleteHeaders();

	void DeleteStructure();

	void AddColumn(const std::string& name);

	Node* AddNode(Node* node, ColumnHeader* colHeader);

	void SetupSudokuConstraints();
	
	void LinkNodesHorizontally(const std::vector<Node*>& nodes);

	void AddSudokuRows(const std::vector<int>& puzzle);

	void SetupSudoku(const std::vector<int>& puzzle);

	bool Solve();

	std::vector<int> GetSolution();
};

