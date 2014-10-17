#include <iostream>
#include "CommandQueue.h"
#include "PdoEntryCache.h"
#include "EtherCatServer.h"
using namespace std;


EtherCatServer::EtherCatServer() {};
EtherCatServer::EtherCatServer(CommandQueue *commandQueueRef, PdoEntryCache *pdoEntryCacheRef) {
  commandQueue = commandQueueRef;
  pdoEntryCache = pdoEntryCacheRef;
};


void EtherCatServer::getDeviceNames() {
  cout << commandQueue->testValue;
};

