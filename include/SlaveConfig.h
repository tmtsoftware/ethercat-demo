#ifndef INCLUDE_SLAVECONFIG_H_
#define INCLUDE_SLAVECONFIG_H_

#include <string>
#include <vector>

#include "PdoEntry.h"
#include "Pdo.h"
#include "SyncManager.h"


class SlaveConfig {
 
  public: SlaveConfig(std::string name, unsigned int vendor_id, unsigned int product_code, unsigned int alias, unsigned int pos);

  public: std::string name;
  public: unsigned int vendor_id;
  public: unsigned int product_code;
  public: unsigned int alias;
  public: unsigned int pos;
  public: unsigned int *domain_offset;
  public: unsigned int *domain_bit_pos;
  public: std::vector<SyncManager> syncs;  	    // full tree for PDO Configuration
  public: std::vector<PdoEntry> pdoEntries;
  public: std::vector<Pdo> pdos;

};

#endif /* INCLUDE_SLAVECONFIG_H_ */
