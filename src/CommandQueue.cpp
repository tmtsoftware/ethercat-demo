#include <iostream>
#include "ecrt.h"
#include "TmtEcStructs.h"
#include <queue>
#include "CommandQueue.h"
using namespace std;


CommandQueue::CommandQueue() {
   cmdQueue = queue<PdoEntryValue>();
};

void CommandQueue::addToQueue(PdoEntryValue pdoEntryValue) {
   cmdQueue.push(pdoEntryValue);
}

PdoEntryValue CommandQueue::getNext() {

   PdoEntryValue pdoEntryValue = cmdQueue.front();
   cmdQueue.pop();

   return pdoEntryValue;
};

bool CommandQueue::isEmpty() {
   return cmdQueue.empty();
};
 
