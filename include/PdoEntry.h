#ifndef INCLUDE_PDOENTRY_H_
#define INCLUDE_PDOENTRY_H_

#include <string>

class PdoEntry {
 
  public: PdoEntry(std::string entryName, std::string pdoName, int index, int subindex, int bitLength);

  public: std::string entryName;
  public: std::string fullName;
  public: uint16_t index;
  public: uint8_t subindex;
  public: uint8_t bitLength;
  public: unsigned int domainOffset;
  public: unsigned int domainBitPos;

};

#endif /* INCLUDE_PDOENTRY_H_ */
