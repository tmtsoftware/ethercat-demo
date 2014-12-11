#ifndef INCLUDE_ETHERCATSERVER_H_
#define INCLUDE_ETHERCATSERVER_H_

#include <string>
#include <vector>

#include "PdoEntryCache.h"
#include "CyclicMotor.h"
#include "ConfigLoader.h"

class EtherCatServer {

private: PdoEntryCache pdoEntryCache;
private: CyclicMotor cyclicMotor;
private: ConfigLoader configLoader;

public: EtherCatServer();
public: void startServer();
public: void stopServer();
public: int configServer(std::string configFile);
public: std::vector<std::string> getDeviceNames();
public: std::vector<std::string> getParameterNames(std::string deviceName);
public: std::string getParameterValue(std::string deviceName, std::string parameterName);
public: void setParameterValue(std::string deviceName, std::string parameterName, int value);
public: void setParameterValue(unsigned int alias, int offset, std::string parameterName, int value);
private: void setParameterValue(std::string deviceName, unsigned int alias, int offset, std::string parameterName, int value);

private: int tempStartup();
};

#endif /* INCLUDE_ETHERCATSERVER_H_ */
