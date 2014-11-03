
using namespace std;

class CommandQueue {
 
  public: queue<PdoEntryValue> cmdQueue;
  public: CommandQueue();

  public: void addToQueue(PdoEntryValue pdoEntryValue);
  public: PdoEntryValue getNext();
  public: bool isEmpty();
};

