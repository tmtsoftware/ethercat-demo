using namespace std;

class PdoEntry {
 
  public: PdoEntry(string entryName, string pdoName, int index, int subindex, int bitLength);

  public: string entryName;
  public: string fullName;
  public: uint16_t index;
  public: uint8_t subindex;
  public: uint8_t bitLength;
  public: unsigned int domainOffset;
  public: unsigned int domainBitPos;

};

