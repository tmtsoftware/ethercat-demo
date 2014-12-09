#include <string>
#include <vector>
#include "ecrt.h"
#include "TmtEcStructs.h"
#include "temp.h"
#include "PdoEntry.h"
#include "Pdo.h"
#include "SyncManager.h"

SyncManager::SyncManager(uint8_t index, ec_direction_t dir, ec_watchdog_mode_t watchdog_mode) {
	this->index = index;
	this->dir = dir;
	this->watchdog_mode = watchdog_mode;
};

