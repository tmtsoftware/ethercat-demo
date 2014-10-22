#include <iostream>
#include "ecrt.h"
#include "TmtEcStructs.h"
#include "CommandQueue.h"
using namespace std;


CommandQueue::CommandQueue() {
   testValue = 66;
};

void CommandQueue::addToQueue(PdoEntryValue pdoEntryValue) {
}

PdoEntryValue CommandQueue::getNext() {

   PdoEntryValue pdoEntryValue;

   return pdoEntryValue;
};
 
