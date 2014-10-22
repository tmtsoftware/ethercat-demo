#include <iostream>
#include "ecrt.h"
#include "TmtEcStructs.h"
#include <vector>
#include "NameMap.h"


NameMap::NameMap() {
};

vector<Device> NameMap::getDevices() {

  Device device;

  vector<Device> deviceList;

  deviceList.push_back(device);

  return deviceList;

};
 
vector<Parameter> NameMap::getParameters(Device device) {

  Parameter parameter;

  vector<Parameter> parameterList;

  parameterList.push_back(parameter);

  return parameterList;
  
};

Parameter NameMap::getParameter(Device device, string parameterName) {
  Parameter parameter;

  return parameter;
};
