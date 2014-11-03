using namespace std;

class EtherCatServer {

private: CommandQueue commandQueue;
private: PdoEntryCache pdoEntryCache;
private: CyclicMotor cyclicMotor;
private: ConfigLoader configLoader;

public: EtherCatServer();
public: void startServer();
public: void stopServer();
public: int configServer(string configFile);
public: vector<string> getDeviceNames();
public: vector<string> getParameterNames(string deviceName);
public: string getParameterValue(string deviceName, string parameterName);
public: void setParameterValue(string deviceName, string parameterName, string value);

private: int tempStartup();
};
