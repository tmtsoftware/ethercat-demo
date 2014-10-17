
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
