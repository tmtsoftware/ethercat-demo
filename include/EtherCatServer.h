using namespace std;

class EtherCatServer {

  private: CommandQueue *commandQueue;
  private: PdoEntryCache *pdoEntryCache;
 
  public: EtherCatServer();
  public: EtherCatServer(CommandQueue *commandQueueRef, PdoEntryCache *pdoEntryCacheRef);


public: void startServer();
public: void stopServer();
public: void configServer(string configFile);
public: vector<string> getDeviceNames();
public: vector<string> getParameterNames(string deviceName);
public: string getParameterValue(string deviceName, string parameterName);
public: void setParameterValue(string deviceName, string parameterName, string value);

};
