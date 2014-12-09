#include <iostream>
#include <iomanip>
#include "ecrt.h"
#include "TmtEcStructs.h"
#include "temp.h"
#include <string>
#include <queue>
#include <vector>
#include <map>
#include "tinystr.h"
#include "tinyxml.h"
#include "PdoEntry.h"
#include "Pdo.h"
#include "SyncManager.h"
#include "SlaveConfig.h"
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

static vector<SlaveConfig> slaves;

/****************************************************************************/



// TODO: this could be in an include file designating the item
#define DigOutSlavePos 1111, 0


/*****************************************************************************/



EtherCatServer::EtherCatServer() {
  pdoEntryCache = PdoEntryCache();
  configLoader = ConfigLoader();
};

void EtherCatServer::startServer() {


	cyclicMotor = CyclicMotor(master,  domain1, domain1_pd, slaves);

	cyclicMotor.start();
};

void EtherCatServer::stopServer() {
	cyclicMotor.stop();
};

int EtherCatServer::configServer(string configFile) {


  // 1. Configure the system

  master = ecrt_request_master(0);
   if (!master)
       return -1;


   // load up the configuration

   slaves = configLoader.loadConfiguration(configFile);

   domain1 = ecrt_master_create_domain(master);
   if (!domain1)
       return -1;

  // load and apply configurations
  configLoader.applyConfiguration(master, domain1, &slaves);

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

// TODO: we need to template this for different types of value
void EtherCatServer::setParameterValue(string deviceName, string parameterName, int value) {

	// TODO: implement as a hashmap of slaves by name containing a hashmap of PdoEntries by fullName.

	//cout << "setParameterValue:: slaves.size() = " << slaves.size();

	for (int si=0; si<slaves.size(); si++) {


		if (deviceName.compare(slaves.at(si).name) == 0) {

			for (int i=0; i<slaves.at(si).pdoEntries.size(); i++) {

				if (parameterName.compare(slaves.at(si).pdoEntries.at(i).fullName) == 0) {
					PdoEntryValue pdoEntryValue = PdoEntryValue();
					pdoEntryValue.pdoEntryIndex = i;
					pdoEntryValue.entryValue = value;
					CommandQueue::instance()->addToQueue(pdoEntryValue);
					cout << "\nslave = " << slaves.at(si).name;
					cout << " ::: " << slaves.at(si).pdoEntries.at(i).fullName << " added to queue";
				}

			}
		}
	}

};



