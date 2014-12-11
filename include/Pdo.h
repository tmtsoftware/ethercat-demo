#ifndef INCLUDE_PDO_H_
#define INCLUDE_PDO_H_

#include <string>
#include <vector>

#include "PdoEntry.h"

class Pdo {
 
public: Pdo(std::string name, int index, int n_entries);

  public: std::string name;
  public: uint16_t index;
  public: unsigned int n_entries;
  public: std::vector<PdoEntry> pdoEntries;

};

#endif /* INCLUDE_PDO_H_ */
