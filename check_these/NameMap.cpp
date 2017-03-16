#include <iostream>
#include "ecrt.h"
#include "TmtEcStructs.h"
#include <vector>
#include "NameMap.h"


NameMap::NameMap() {
};

std::vector<Device> NameMap::getDevices() {

  Device device;

  std::vector<Device> deviceList;

  deviceList.push_back(device);

  return deviceList;

};
 
std::vector<Parameter> NameMap::getParameters(Device device) {

  Parameter parameter;

  std::vector<Parameter> parameterList;

  parameterList.push_back(parameter);

  return parameterList;
  
};

Parameter NameMap::getParameter(Device device, std::string parameterName) {
  Parameter parameter;

  return parameter;
};
