
struct PdoEntryValue {
  unsigned int pdoEntryIndex;
  unsigned int entryValue;
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
