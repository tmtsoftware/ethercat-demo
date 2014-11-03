#include <iostream>
#include "ecrt.h"
#include "TmtEcStructs.h"
#include "temp.h"
#include <string>
#include <queue>
#include <vector>
#include "PdoEntryCache.h"
#include "CommandQueue.h"
#include "CyclicMotor.h"
#include "ConfigLoader.h"
#include "EtherCatServer.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <thread>

using namespace std;


/****************************************************************************/

// Application parameters

#define PRIORITY 1

/****************************************************************************/

// offsets for PDO entries
// TODO: this may be in the include file as well, not sure....
static unsigned int off_dig_out[4];
static unsigned int bp_dig_out[4];

// process data
static uint8_t *domain1_pd = NULL;


// EtherCAT
static ec_master_t *master = NULL;
static ec_master_state_t master_state = {};

static ec_domain_t *domain1 = NULL;
static ec_domain_state_t domain1_state = {};

static ec_slave_config_t *sc_ana_in = NULL;
static ec_slave_config_state_t sc_ana_in_state = {};

// Timer
static unsigned int sig_alarms = 0;
static unsigned int user_alarms = 0;

/****************************************************************************/


// TODO: this could be in an include file designating the item
#define DigOutSlavePos 1111, 0

// TODO: this could be in an include file designating all devices
#define Beckhoff_EK1100 0x00000002, 0x044c2c52
#define Beckhoff_EL2202 0x00000002, 0x089a3052

/* Master 0, Slave 1, "EL2202"
 * Vendor ID:       0x00000002
 * Product code:    0x089a3052
 * Revision number: 0x00100000
 **/

ec_pdo_entry_info_t slave_1_pdo_entries[] = {
    {0x7000, 0x01, 1}, /* Output */
    {0x7000, 0x02, 1}, /* TriState */
    {0x7010, 0x01, 1}, /* Output */
    {0x7010, 0x02, 1}, /* TriState */
};

ec_pdo_info_t slave_1_pdos[] = {
    {0x1600, 2, slave_1_pdo_entries + 0}, /* Channel 1 */
    {0x1601, 2, slave_1_pdo_entries + 2}, /* Channel 2 */
};

ec_sync_info_t slave_1_syncs[] = {
    {0, EC_DIR_OUTPUT, 2, slave_1_pdos + 0, EC_WD_ENABLE},
    {0xff},
};

/*****************************************************************************/

tmt_ec_slave_t tmt_ec_slave[] = {

{"DigitalOut",
0x00000002,
0x089a3052,
1111,
0,
off_dig_out,
bp_dig_out,
slave_1_syncs,
slave_1_pdos,
slave_1_pdo_entries,
{"Channel 1 Output", "Channel 1 TriState", "Channel 2 Output", "Channel 2 TriState"},
4,
},
};

/*****************************************************************************/



EtherCatServer::EtherCatServer() {
  pdoEntryCache = PdoEntryCache();
  commandQueue = CommandQueue();
  configLoader = ConfigLoader();
};

void EtherCatServer::startServer() {

	cyclicMotor = CyclicMotor(master,  domain1, off_dig_out, bp_dig_out, domain1_pd);

	cyclicMotor.start();
};

void EtherCatServer::stopServer() {
	cyclicMotor.stop();
};

int EtherCatServer::configServer(string configFile) {

  // 1. Configure the system
  //vector<SlaveConfig> slaveConfigList = configLoader.loadConfiguration();

  master = ecrt_request_master(0);
   if (!master)
       return -1;

   domain1 = ecrt_master_create_domain(master);
   if (!domain1)
       return -1;

  // load and apply configurations
  ConfigLoader configLoader = ConfigLoader();
  configLoader.loadConfiguration(master, tmt_ec_slave, domain1, off_dig_out, bp_dig_out);

  printf("Activating master...\n");
  if (ecrt_master_activate(master))
      return -1;

  if (!(domain1_pd = ecrt_domain_data(domain1))) {
      return -1;
  }

  return 0;

};

vector<string> EtherCatServer::getDeviceNames() {
  vector<string> test;
  test.push_back("one");
  test.push_back("two");
  return test;
};


vector<string> EtherCatServer::getParameterNames(string deviceName) {
 
  vector<string> test;
  test.push_back(deviceName + "::one");
  test.push_back(deviceName + "::two");
  return test;
};

// TODO: should this be templated (we need to learn about templating) to 
// instead return 'int', 'float', etc??
string EtherCatServer::getParameterValue(string deviceName, string parameterName) {
 
  return deviceName + "::" + parameterName + "::" + "value";
};

void EtherCatServer::setParameterValue(string deviceName, string parameterName, string value) {

};



