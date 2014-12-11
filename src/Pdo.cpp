#include <string>

#include "Pdo.h"

Pdo::Pdo(std::string name, int index, int n_entries) {
	this->name = name;
	this->index = index;
	this->n_entries = n_entries;
};
