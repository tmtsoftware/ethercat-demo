#include <iostream>
#include "ecrt.h"
#include "TmtEcStructs.h"
#include "PdoEntryCache.h"
#include "CommandQueue.h"
#include "EtherCatServer.h"
#include "CyclicMotor.h"
#include "ConfigLoader.h"

int main()
{
  PdoEntryCache pdoEntryCache = PdoEntryCache();
  CommandQueue commandQueue = CommandQueue();
  CyclicMotor cyclicMotor = CyclicMotor();
  ConfigLoader configLoader = ConfigLoader();

  EtherCatServer etherCatServer = EtherCatServer(&commandQueue, &pdoEntryCache);
  etherCatServer.getDeviceNames();
  std::cout << "Hello World!";

}
