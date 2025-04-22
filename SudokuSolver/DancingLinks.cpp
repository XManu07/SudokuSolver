#include "DancingLinks.h"

DancingLinks::DancingLinks() : solutionFound(false){
	header = new ColumnHeader("header");
}

DancingLinks::~DancingLinks() {
	DeleteSudokuStructure();
	delete header;
}

// search for the column with the least number of 1s
ColumnHeader* DancingLinks::ChooseColumn() {
	if (header->right == header) {
		return nullptr;
	}

	ColumnHeader* bestColumn = (ColumnHeader*)header->right;
	int minSize = bestColumn->size;

	for (Node* col = header->right; col != header; col = col->right) {
		ColumnHeader* columnHeader = (ColumnHeader*)col;
		if (columnHeader->size < minSize) {
			minSize = columnHeader->size;
			bestColumn = columnHeader;

			if (minSize == 1)
				break;
		}   
	}

	return bestColumn;
}

void DancingLinks::CoverColumn(ColumnHeader* col)
{
	col->UnlinkHorizontal();

	for (Node* row = col->down; row != col; row = row->down) {
		for (Node* cell = row->right; cell != row; cell = cell->right) {
			cell->UnlinkVertical();
			static_cast<ColumnHeader*>(cell->column)->size--;
		}
	}
}

void DancingLinks::UncoverColumn(ColumnHeader* col)
{
	for (Node* row = col->up; row != col; row = row->up) {
		for (Node* cell = row->left; cell != row; cell = cell->left) {
			cell->RelinkVertical();
			static_cast<ColumnHeader*>(cell->column)->size++;
		}
	}

	col->RelinkHorizontal();
}

bool DancingLinks::SearchForSolution(int depth)
{
	if (header->right == header) {
		solutionFound = true;
		return true;
	}

	ColumnHeader* col = ChooseColumn();
	if (!col) return false;

	CoverColumn(col);

	for (Node* row = col->down; row != col && row != nullptr; row = row->down) {
		solution.push_back(row);

		for (Node* cell = row->right; cell != row && cell != nullptr; cell = cell->right) {
			if (cell && cell->column) {
				CoverColumn(static_cast<ColumnHeader*>(cell->column));
			}
		}

		if (SearchForSolution(depth + 1)) {
			return true;
		}

		solution.pop_back();

		for (Node* cell = row->left; cell != row && cell != nullptr; cell = cell->left) {
			if (cell && cell->column) {
				UncoverColumn(static_cast<ColumnHeader*>(cell->column));
			}
		}
	}

	UncoverColumn(col);

	return false;
}

void DancingLinks::DeleteHeaders()
{
	for (auto& colHeader : columnHeaders) {
		delete colHeader.second;
	}
}

void DancingLinks::RelinkHeaders()
{
	header->left = header;
	header->right = header;

	for (auto& colHeader : columnHeaders) {
		colHeader.second->left = header->left;
		colHeader.second->right = header;
		header->left->right = colHeader.second;
		header->left = colHeader.second;

		colHeader.second->up = colHeader.second;
		colHeader.second->down = colHeader.second;

		colHeader.second->size = 9;
	}

}

void DancingLinks::RelinkNodes()
{

	for (unsigned int i = 0; i < allNodes.size(); i+=4) {
		std::vector<Node*> nodesToLink;

		nodesToLink.push_back(allNodes[i]);
		LinkNodeVertical(allNodes[i]);

		nodesToLink.push_back(allNodes[i + 1]);
		LinkNodeVertical(allNodes[i + 1]);

		nodesToLink.push_back(allNodes[i + 2]);
		LinkNodeVertical(allNodes[i + 2]);

		nodesToLink.push_back(allNodes[i + 3]);
		LinkNodeVertical(allNodes[i + 3]);

		LinkNodesHorizontally(nodesToLink);
	}
}

// reset each node link to its original state
void DancingLinks::ResetStructure()
{
	RelinkHeaders();
	RelinkNodes();
	solution.clear();
	solutionFound = false;
}

void DancingLinks::DeleteSudokuStructure() {
	solution.clear();
	for (Node* node : allNodes) {
		delete node;
	}
	DeleteHeaders();
	columnHeaders.clear();
	allNodes.clear();
}

void DancingLinks::AddColumn(const std::string& name)
{
	ColumnHeader* colHeader = new ColumnHeader(name);

	colHeader->left = header->left;
	colHeader->right = header;
	header->left->right = colHeader;
	header->left = colHeader;

	columnHeaders[name] = colHeader;

}

Node* DancingLinks::AddNode(Node* node, ColumnHeader* colHeader)
{ 
	node->column = colHeader;
	node->up = colHeader->up;
	node->down = colHeader;
	colHeader->up->down = node;
	colHeader->up = node;

	colHeader->size++;

	allNodes.push_back(node);

	return node;
}

void DancingLinks::SetupSudokuConstraints()
{
	// cell constraints
	for (int row = 0; row < 9; row++) {
		for (int col = 0; col < 9; col++) {
			AddColumn("cell_" + std::to_string(row) + "_" + std::to_string(col));
		}
	}
	
	// row constraints
	for (int row = 0; row < 9; row++) {
		for (int digit = 1; digit <= 9; digit++) {
			AddColumn("row_" + std::to_string(row) + "_" + std::to_string(digit));
		}
	}

	// column constraints
	for (int col = 0; col < 9; col++) {
		for (int digit = 1; digit <= 9; digit++) {
			AddColumn("col_" + std::to_string(col) + "_" + std::to_string(digit));
		}
	}

	// box constraints
	for (int box = 0; box < 9; box++) {
		for (int digit = 1; digit <= 9; digit++) {
			AddColumn("box_" + std::to_string(box) + "_" + std::to_string(digit));
		}
	}


}

void DancingLinks::CoverExistingConstraints(const std::vector<int>& puzzle)
{
	// Clear any existing solution first to avoid duplicates
	solution.clear();

	for (int r = 0; r < 9; r++) {
		for (int c = 0; c < 9; c++) {
			int digit = puzzle[r * 9 + c];
			if (digit > 0) {  // If this is a given digit
				// Get the column header for this cell
				std::string cellKey = "cell_" + std::to_string(r) + "_" + std::to_string(c);
				ColumnHeader* cellHeader = columnHeaders[cellKey];
				bool foundNode = false;

				// Find the node corresponding to this digit in this cell
				for (Node* node = cellHeader->down; node != cellHeader; node = node->down) {
					if (node->digit == digit) {
						// Found the right node, add it to solution
						solution.push_back(node);
						foundNode = true;

						// Cover this node's column (the cell constraint)
						CoverColumn(static_cast<ColumnHeader*>(node->column));

						// Cover all other constraints this node satisfies
						for (Node* cell = node->right; cell != node; cell = cell->right) {
							CoverColumn(static_cast<ColumnHeader*>(cell->column));
						}

						break;  // Found and processed this given digit
					}
				}
			}
		}
	}
}


void DancingLinks::LinkNodesHorizontally(const std::vector<Node*>& nodes)
{
	unsigned int n = nodes.size();
	for (unsigned int i = 0; i < n; i++) {

		unsigned int leftIndex = (i - 1 + n) % n;
		unsigned int rightIndex = (i + 1) % n;

		nodes[i]->left = nodes[leftIndex];
		nodes[i]->right = nodes[rightIndex];

	}
}
void DancingLinks::LinkNodeVertical(Node* node)
{
	node->down = node->column;
	node->up = node->column->up;
	node->column->up->down = node;
	node->column->up = node;
}


void DancingLinks::AddSudokuRows()
{
	for (int row = 0; row < 9; row++) {
		for (int col = 0; col < 9; col++) {
			int boxIndex = (row / 3) * 3 + (col / 3);

			for (int digit = 1; digit <= 9; digit++) {
				std::string cellKey = "cell_" + std::to_string(row) + "_" + std::to_string(col);
				std::string rowKey = "row_" + std::to_string(row) + "_" + std::to_string(digit);
				std::string colKey = "col_" + std::to_string(col) + "_" + std::to_string(digit);
				std::string boxKey = "box_" + std::to_string(boxIndex) + "_" + std::to_string(digit);

				std::vector<Node*> rowNodes;

				Node* cellNode = new Node(row, col, digit);
				rowNodes.push_back(AddNode(cellNode, columnHeaders[cellKey]));

				Node* rowNode = new Node(row, col, digit);
				rowNodes.push_back(AddNode(rowNode, columnHeaders[rowKey]));

				Node* colNode = new Node(row, col, digit);
				rowNodes.push_back(AddNode(colNode, columnHeaders[colKey]));

				Node* boxNode = new Node(row, col, digit);
				rowNodes.push_back(AddNode(boxNode, columnHeaders[boxKey]));

				LinkNodesHorizontally(rowNodes);
			}
		}
	}
}

void DancingLinks::SetupSudokuStructure()
{
	SetupSudokuConstraints();
	AddSudokuRows();
}

bool DancingLinks::Solve()
{
	solutionFound = false;
	return SearchForSolution();
}

std::vector<int> DancingLinks::GetSolution()
{
	std::vector<int> grid(81, 0);

	for (Node* node : solution) {
		int row = node->row;
		int col = node->col;
		int digit = node->digit;

		if (row >= 0 && row < 9 && col >= 0 && col < 9 && digit >= 1 && digit <= 9) {
			grid[row * 9 + col] = digit;
		}
	}
	return grid;
}
