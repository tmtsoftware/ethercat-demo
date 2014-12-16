
#ifndef INCLUDE_PDOENTRYCACHE_H_
#define INCLUDE_PDOENTRYCACHE_H_

#include "TmtEcStructs.h"
#include <map>

class PdoEntryCache {

  private: std::map<int, int> cacheMap;

  public: static PdoEntryCache* instance();

  private: static PdoEntryCache* pInstance;

  public: void updatePdoEntryValue(int slaveId, int pdoEntryId, int value);
  public: int getPdoEntryValue(int slaveId, int pdoEntryId);
};

#endif /* INCLUDE_PDOENTRYCACHE_H_ */
