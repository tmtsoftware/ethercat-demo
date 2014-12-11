#ifndef INCLUDE_SYNCMANAGER_H_
#define INCLUDE_SYNCMANAGER_H_

#include <vector>

#include "ecrt.h"
#include "Pdo.h"

class SyncManager {
 
  public: SyncManager(uint8_t index, ec_direction_t dir, ec_watchdog_mode_t watchdog_mode);

  public: uint8_t index;
  public: ec_direction_t dir;
  public: ec_watchdog_mode_t watchdog_mode;
  public: std::vector<Pdo> pdos;

};

#endif /* INCLUDE_SYNCMANAGER_H_ */
