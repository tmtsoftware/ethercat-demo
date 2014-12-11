#ifndef INCLUDE_CONFIGLOADER_H_
#define INCLUDE_CONFIGLOADER_H_

#include "ecrt.h"
#include "tinystr.h"
#include "tinyxml.h"
#include <string>
#include <vector>
#include "SlaveConfig.h"


class ConfigLoader {
 
  public: ConfigLoader();

  public: void applyConfiguration(ec_master_t *master, ec_domain *domain1, std::vector<SlaveConfig> *slaveConfigVector);

  public: std::vector<SlaveConfig> loadConfiguration(ec_master_t *master, std::string configFile);

  private: std::vector<Pdo> loadPdoInfo(const char* type, TiXmlElement *deviceElem, int smIndex, int n_pdos);

  private: unsigned int charToInt(const char* input);
  private: unsigned int hexCharToInt(const char* input);

  private: int countChildren(TiXmlNode* parent);

  private: int countChildren(TiXmlNode* parent, const char* name);
  private: int countPdoChildren(TiXmlNode* parent, const char* name, int smIndex);

  private: void printSyncs(int nSyncs, ec_sync_info_t *sync);
};

#endif /* INCLUDE_CONFIGLOADER_H_ */
