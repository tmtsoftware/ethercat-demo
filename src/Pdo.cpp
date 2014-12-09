#include <string>
#include <vector>
#include "ecrt.h"
#include "TmtEcStructs.h"
#include "temp.h"
#include "PdoEntry.h"
#include "Pdo.h"


Pdo::Pdo(string name, int index, int n_entries) {
	this->name = name;
	this->index = index;
	this->n_entries = n_entries;
};
