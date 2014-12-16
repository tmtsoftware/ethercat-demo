#include <iostream>
#include <map>
#include "ecrt.h"
#include "TmtEcStructs.h"
#include "PdoEntryCache.h"


// Singleton pattern stuff

PdoEntryCache* PdoEntryCache::pInstance = 0;

PdoEntryCache* PdoEntryCache::instance() {
   if (pInstance == 0) {
      pInstance = new PdoEntryCache;
   }
   return pInstance;
}

void PdoEntryCache::updatePdoEntryValue(int slaveId, int pdoEntryId, int pdoEntryValue) {
	int key = (slaveId * 1000) + pdoEntryId;
	cacheMap[key] = pdoEntryValue;
};

int PdoEntryCache::getPdoEntryValue(int slaveId, int pdoEntryId) {
	int key = (slaveId * 1000) + pdoEntryId;

	//std::cout << "\nKEY = " + key;

  return cacheMap[key];

};
