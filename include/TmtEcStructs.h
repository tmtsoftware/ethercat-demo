
#ifndef INCLUDE_TMTECSTRUCTS_H_
#define INCLUDE_TMTECSTRUCTS_H_

struct PdoEntryValue {
  unsigned int slaveIndex;
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

#endif /* INCLUDE_TMTECSTRUCTS_H_ */
