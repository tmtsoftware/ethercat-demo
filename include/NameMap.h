using namespace std;

class NameMap {
 
  public: NameMap();

  public: vector<Device> getDevices();

  public: vector<Parameter> getParameters(Device device);

  public: Parameter getParameter(Device device, string paramName);

};

