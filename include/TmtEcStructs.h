
struct PdoEntryValue {
  unsigned int pdoEntryIndex;
  unsigned int entryValue;
};

struct SlaveConfig {
  int slaveId;
  PdoEntryValue pdoEntryValue;
};

struct Device {
  int deviceId;
  int slaveId;
  char *deviceName;
};

struct Parameter {
   int parameterId;
   int slaveId;
   int pdoEntryId;
   char *parameterName;
};
