
#ifndef INCLUDE_PDOENTRYCACHE_H_
#define INCLUDE_PDOENTRYCACHE_H_

#include "TmtEcStructs.h"

class PdoEntryCache {

 
  public: PdoEntryCache();

  public: void updatePdoEntryValue(int pdoEntryId, PdoEntryValue pdoEntryValue);
  public: PdoEntryValue getPdoEntryValue(int pdoEntryId);
};

#endif /* INCLUDE_PDOENTRYCACHE_H_ */
