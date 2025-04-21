#pragma once
#include <cstdint>

class Node
{
public:
	Node* left;
	Node* right;
	Node* up;
	Node* down;

	class ColumnHeader* column;
	
	int row;
	int col;
	int digit;

	Node() : left(this), right(this), up(this), down(this), column(nullptr), row(-1), col(-1), digit(-1) {}

	Node(int r, int c, int d)
		:left(this), right(this), up(this), down(this), column(nullptr), row(r), col(c), digit(d) {}

	void UnlinkHorizontal();

	void RelinkHorizontal();

	void UnlinkVertical();

	void RelinkVertical();
};

