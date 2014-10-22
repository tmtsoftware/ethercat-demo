class CommandQueue {
 
  public: int testValue;
  public: CommandQueue();

  public: void addToQueue(PdoEntryValue pdoEntryValue);
  public: PdoEntryValue getNext();
};

