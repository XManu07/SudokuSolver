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
	ColumnHeader* header;
	std::unordered_map<std::string, ColumnHeader*> columnHeaders;
	std::vector<Node*> solution;
	std::vector<Node*> allNodes;
	bool solutionFound;


public:
	DancingLinks();

	~DancingLinks();

	ColumnHeader* ChooseColumn();

	void CoverColumn(ColumnHeader* col);

	void UncoverColumn(ColumnHeader* col);

	bool SearchForSolution(int depth = 0);

	void DeleteHeaders();

	void RelinkHeaders();

	void RelinkNodes();

	void ResetStructure();

	void DeleteSudokuStructure();

	void AddColumn(const std::string& name);

	Node* AddNode(Node* node, ColumnHeader* colHeader);

	void SetupSudokuConstraints();

	void CoverExistingConstraints(const std::vector<int>& puzzle);
	
	void AddSudokuRows();

	void LinkNodesHorizontally(const std::vector<Node*>& nodes);

	void LinkNodeVertical(Node* node);

	void SetupSudokuStructure();

	bool Solve();

	std::vector<int> GetSolution();
};

