#ifndef INCLUDE_NAMEMAP_H_
#define INCLUDE_NAMEMAP_H_

#include <string>
#include <vector>
#include "TmtEcStructs.h"

class NameMap {
 
  public: NameMap();

  public: std::vector<Device> getDevices();

  public: std::vector<Parameter> getParameters(Device device);

  public: Parameter getParameter(Device device, std::string paramName);

};

#endif /* INCLUDE_NAMEMAP_H_ */
