#include "ecrt.h"
#include "SyncManager.h"

SyncManager::SyncManager(uint8_t index, ec_direction_t dir, ec_watchdog_mode_t watchdog_mode) {
	this->index = index;
	this->dir = dir;
	this->watchdog_mode = watchdog_mode;
};

