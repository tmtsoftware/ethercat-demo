
using namespace std;

class CommandQueue {
 
  public: queue<PdoEntryValue> cmdQueue;
  private: CommandQueue();

  public: static CommandQueue* instance();

  private: static CommandQueue* pInstance;

  public: void addToQueue(PdoEntryValue pdoEntryValue);
  public: PdoEntryValue getNext();
  public: bool isEmpty();
};

