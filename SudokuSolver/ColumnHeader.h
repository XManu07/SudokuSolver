#include <iostream>
#include "Node.h"
#pragma once

class ColumnHeader : public Node {
public:
	int size;
	std::string name;	

	ColumnHeader(const std::string& name)
		: Node(), size(0), name(name) {
		column = this;
	}
};

