#include <iostream>
#include "ecrt.h"
#include "TmtEcStructs.h"
#include "CommandQueue.h"
#include "PdoEntryCache.h"
#include <string>
#include <vector>
#include "EtherCatServer.h"

EtherCatServer::EtherCatServer() {};
EtherCatServer::EtherCatServer(CommandQueue *commandQueueRef, PdoEntryCache *pdoEntryCacheRef) {
  commandQueue = commandQueueRef;
  pdoEntryCache = pdoEntryCacheRef;
};

void EtherCatServer::startServer() {

};

void EtherCatServer::stopServer() {

};

void EtherCatServer::configServer(string configFile) {

};

vector<string> EtherCatServer::getDeviceNames() {
  cout << commandQueue->testValue;

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
