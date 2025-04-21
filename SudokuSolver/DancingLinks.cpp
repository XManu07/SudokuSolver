#include "DancingLinks.h"

DancingLinks::DancingLinks() : solutionFound(false){
	header = new ColumnHeader("header");
}

DancingLinks::~DancingLinks() {
	DeleteStructure();
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

	for (Node* row = col->down; row != col; row = row->down) {
		solution.push_back(row);

		for (Node* cell = row->right; cell != row; cell = cell->right) {
			if (cell && cell->column) {
				CoverColumn(static_cast<ColumnHeader*>(cell->column));
			}
		}

		if (SearchForSolution(depth + 1)) {
			return true;
		}

		solution.pop_back();

		for (Node* cell = row->left; cell != row; cell = cell->left) {
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
	for (auto& pair : columnHeaders) {
		delete pair.second;
	}
}

void DancingLinks::DeleteStructure() {
	for (Node* node : allNodes) {
		delete node;
	}
	DeleteHeaders();
	columnHeaders.clear();
	solution.clear();
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
	if (!colHeader) 
		return nullptr;

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

void DancingLinks::LinkNodesHorizontally(const std::vector<Node*>& nodes)
{
	unsigned int n = nodes.size();
	for (unsigned int i = 0; i < n; i++) {

		unsigned int leftIdx = i;
		unsigned int rightIdx = i;

		// Find the next valid node to the left
		do {
			leftIdx = (leftIdx + n - 1) % n;
		} while (leftIdx != i && !nodes[leftIdx]);

		// Find the next valid node to the right
		do {
			rightIdx = (rightIdx + 1) % n;
		} while (rightIdx != i && !nodes[rightIdx]);

		// Link nodes if both left and right nodes are valid
		if (nodes[leftIdx] && nodes[rightIdx]) {
			nodes[i]->left = nodes[leftIdx];
			nodes[i]->right = nodes[rightIdx];
		}
	}
}

void DancingLinks::AddSudokuRows(const std::vector<int>& puzzle)
{
	for (int row = 0; row < 9; row++) {
		for (int col = 0; col < 9; col++) {
			int cellIndex = row * 9 + col;
			int boxIndex = (row / 3) * 3 + (col / 3);

			std::vector<int> digits;
			if (puzzle[cellIndex] != 0) {
				digits.push_back(puzzle[cellIndex]);
			}
			else {
				for (int digit = 1; digit <= 9; digit++) {
					digits.push_back(digit);
				}
			}

			for (int digit : digits) {
				// Get the constraint column headers
				std::string cellKey = "cell_" + std::to_string(row) + "_" + std::to_string(col);
				std::string rowKey = "row_" + std::to_string(row) + "_" + std::to_string(digit);
				std::string colKey = "col_" + std::to_string(col) + "_" + std::to_string(digit);
				std::string boxKey = "box_" + std::to_string(boxIndex) + "_" + std::to_string(digit);

				// Create nodes for each constraint this placement satisfies
				std::vector<Node*> rowNodes;

				// Cell constraint
				Node* mainNode = new Node(row, col, digit);
				rowNodes.push_back(AddNode(mainNode, columnHeaders[cellKey]));

				// Row constraint
				Node* rowNode = new Node(row, col, digit);
				rowNodes.push_back(AddNode(rowNode, columnHeaders[rowKey]));

				// Column constraint
				Node* colNode = new Node(row, col, digit);
				rowNodes.push_back(AddNode(colNode, columnHeaders[colKey]));

				// Box constraint
				Node* boxNode = new Node(row, col, digit);
				rowNodes.push_back(AddNode(boxNode, columnHeaders[boxKey]));

				// Link all nodes for this row
				LinkNodesHorizontally(rowNodes);
			}
		}
	}
}

void DancingLinks::SetupSudoku(const std::vector<int>& puzzle)
{
	DeleteStructure();
	SetupSudokuConstraints();
	AddSudokuRows(puzzle);
}

bool DancingLinks::Solve()
{
	solution.clear();
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
