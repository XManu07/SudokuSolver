#include "Node.h"

void Node::UnlinkHorizontal()
{
	left->right = right;
	right->left = left;
}

void Node::RelinkHorizontal()
{
	left->right = this;
	right->left = this;
}

void Node::UnlinkVertical()
{
	up->down = down;
	down->up = up;
}

void Node::RelinkVertical()
{
	up->down = this;
	down->up = this;
}

