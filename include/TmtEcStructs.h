
struct PdoEntryValue {
  ec_pdo_entry_info_t pdoEntry;
  unsigned int domainOffset;
  unsigned int domainBitPos;
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
