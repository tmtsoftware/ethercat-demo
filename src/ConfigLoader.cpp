#include <iostream>
#include "ecrt.h"
#include "TmtEcStructs.h"
#include <vector>
#include "ConfigLoader.h"
using namespace std;


ConfigLoader::ConfigLoader() {
};

vector<SlaveConfig> ConfigLoader::loadConfiguration() {

  SlaveConfig slaveConfig;

  vector<SlaveConfig> slaveConfigList;

  slaveConfigList.push_back(slaveConfig);

  return slaveConfigList;

};
 
