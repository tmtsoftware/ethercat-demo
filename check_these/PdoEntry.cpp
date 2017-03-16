#include <iostream>
#include <sstream>
#include <string>

#include "PdoEntry.h"

PdoEntry::PdoEntry(std::string entryName, std::string pdoName, int index, int subindex, int bitLength) {

	this->entryName = entryName;

	// create the full name
	std::stringstream ss;
	ss << pdoName << "::" << entryName;
	this->fullName = ss.str();

	this->index = index;
	this->subindex = subindex;
	this->bitLength = bitLength;
	this->domainOffset = 0;
	this->domainBitPos = 0;

};
