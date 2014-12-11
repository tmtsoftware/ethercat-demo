#ifndef INCLUDE_COMMANDQUEUE_H_
#define INCLUDE_COMMANDQUEUE_H_

#include <queue>

#include "TmtEcStructs.h"

class CommandQueue {
 
  public: std::queue<PdoEntryValue> cmdQueue;
  private: CommandQueue();

  public: static CommandQueue* instance();

  private: static CommandQueue* pInstance;

  public: void addToQueue(PdoEntryValue pdoEntryValue);
  public: PdoEntryValue getNext();
  public: bool isEmpty();
};

#endif /* INCLUDE_COMMANDQUEUE_H_ */
