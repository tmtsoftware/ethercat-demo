
#include <iostream>
#include "ecrt.h"

#include <string>
#include <vector>
#include "TmtEcStructs.h"
#include "SlaveConfig.h"
#include "CommandQueue.h"
#include "ConfigLoader.h"

#include "EtherCatServer.h"


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


static std::vector<SlaveConfig> slaves;

/****************************************************************************/

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

int EtherCatServer::configServer(std::string configFile) {


  // 1. Configure the system

  master = ecrt_request_master(0);
   if (!master)
       return -1;


   // load up the configuration

   slaves = configLoader.loadConfiguration(master, configFile);

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

std::vector<std::string> EtherCatServer::getDeviceNames() {

  std::vector<std::string> deviceNameList;
  for (int si=0; si<(int)slaves.size(); si++) {
	  deviceNameList.push_back(slaves.at(si).name);
  }
  return deviceNameList;

};

// FIXME - this does not work when two device names are the same
std::vector<std::string> EtherCatServer::getParameterNames(std::string deviceName) {
 
  std::vector<std::string> parameterNameList;
    for (int si=0; si<(int)slaves.size(); si++) {
		if (deviceName.compare(slaves.at(si).name) == 0) {
    	  for (unsigned int i=0; i<slaves.at(si).pdoEntries.size(); i++) {
        	parameterNameList.push_back(slaves.at(si).pdoEntries.at(i).fullName);
    	  }
		}
	}
  return parameterNameList;

};

// TODO: should this be templated (we need to learn about templating) to 
// instead return 'int', 'float', etc??
int EtherCatServer::getParameterValue(std::string deviceName, std::string parameterName) {
 
  return getParameterValue(deviceName, 0, 0, parameterName);
};

int EtherCatServer::getParameterValue(unsigned int deviceAlias, int devicePosition, std::string parameterName) {
	return getParameterValue("", deviceAlias, devicePosition, parameterName);
}

int EtherCatServer::getParameterValue(std::string deviceName, unsigned int alias, int pos, std::string parameterName) {

	// determine server
	int slaveIndex = -1;
	int entryIndex = -1;
	if (!deviceName.empty()) {
		// TODO: this should be a hashmap
		for (int si=0; si<(int)slaves.size(); si++) {
			if (deviceName.compare(slaves.at(si).name) == 0) {
				slaveIndex = si;
			}
		}
	} else if (alias == 0) {
		slaveIndex = pos;
	} else {
		// handle aliases
		for (int si=0; si<(int)slaves.size(); si++) {
			if (slaves.at(si).alias == alias) {
				slaveIndex = si;
			}
		}

	}

	SlaveConfig slaveConfig = slaves.at(slaveIndex);
	// TODO: should be a hashmap
	for (int i=0; i<slaveConfig.pdoEntries.size(); i++) {
		if (parameterName.compare(slaveConfig.pdoEntries.at(i).fullName) == 0) {
			entryIndex = i;
		}
	}

	std::cout << "\nslave, entry = " << slaveIndex << ", " << entryIndex;
	return PdoEntryCache::instance()->getPdoEntryValue(slaveIndex, entryIndex);
}

// TODO: we need to template this for different types of value
void EtherCatServer::setParameterValue(std::string deviceName, std::string parameterName, int value) {
	setParameterValue(deviceName, 0, 0, parameterName, value);
}

void EtherCatServer::setParameterValue(unsigned int deviceAlias, int deviceOffset, std::string parameterName, int value) {
	setParameterValue("", deviceAlias, deviceOffset, parameterName, value);
}



void EtherCatServer::setParameterValue(std::string deviceName, unsigned int alias, int pos, std::string parameterName, int value) {

	// TODO: implement as a hashmap of slaves by name containing a hashmap of PdoEntries by fullName.

	//cout << "setParameterValue:: slaves.size() = " << slaves.size();

	for (int si=0; si<(int)slaves.size(); si++) {


		//std::cout << slaves.at(si).name << "::" << deviceName << "::" << alias;

		if ((deviceName.empty() && (alias == slaves.at(si).alias || (alias == 0U && si == pos)))
				|| (deviceName.compare(slaves.at(si).name) == 0 && (alias == 0U || alias == slaves.at(si).alias))) {

			for (int i=0; i<(int)slaves.at(si).pdoEntries.size(); i++) {

				if (parameterName.compare(slaves.at(si).pdoEntries.at(i).fullName) == 0) {
					PdoEntryValue pdoEntryValue = PdoEntryValue();
					pdoEntryValue.pdoEntryIndex = i;
					pdoEntryValue.slaveIndex = si;
					pdoEntryValue.entryValue = value;
					CommandQueue::instance()->addToQueue(pdoEntryValue);
					std::cout << "\nslave = " << slaves.at(si).name;
					std::cout << " ::: " << slaves.at(si).pdoEntries.at(i).fullName << ", value = " << pdoEntryValue.entryValue << " added to queue";

				}

			}
		}
	}

};



