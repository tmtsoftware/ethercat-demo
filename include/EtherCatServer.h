class EtherCatServer {

  private: CommandQueue *commandQueue;
  private: PdoEntryCache *pdoEntryCache;
 
  public: EtherCatServer();
  public: EtherCatServer(CommandQueue *commandQueueRef, PdoEntryCache *pdoEntryCacheRef);


public: void getDeviceNames();

};
