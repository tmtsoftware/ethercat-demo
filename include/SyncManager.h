using namespace std;

class SyncManager {
 
  public: SyncManager(uint8_t index, ec_direction_t dir, ec_watchdog_mode_t watchdog_mode);

  public: uint8_t index;
  public: ec_direction_t dir;
  public: ec_watchdog_mode_t watchdog_mode;
  public: vector<Pdo> pdos;

};

