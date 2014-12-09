#include <iostream>
#include <sstream>
#include <string>
#include "ecrt.h"
#include "TmtEcStructs.h"
#include "temp.h"
#include "PdoEntry.h"

PdoEntry::PdoEntry(string entryName, string pdoName, int index, int subindex, int bitLength) {

	this->entryName = entryName;

	// create the full name
	std::stringstream ss;
	ss << pdoName << "::" << entryName;
	this->fullName = ss.str();
	cout << "\n full name = " << fullName;

	this->index = index;
	this->subindex = subindex;
	this->bitLength = bitLength;
	this->domainOffset = 0;
	this->domainBitPos = 0;
};
