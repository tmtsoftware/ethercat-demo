#include <iostream>

#include "TmtEcStructs.h"
#include <queue>
#include "CommandQueue.h"


// Singleton pattern stuff

CommandQueue* CommandQueue::pInstance = 0;

CommandQueue* CommandQueue::instance() {
   if (pInstance == 0) {
      pInstance = new CommandQueue;
   }
   return pInstance;
}

CommandQueue::CommandQueue() {
   cmdQueue = std::queue<PdoEntryValue>();
};

// TODO: this class is not thread-safe.  We need to implement locking.

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
 
