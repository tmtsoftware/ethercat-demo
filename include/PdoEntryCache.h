class PdoEntryCache {

 
  public: PdoEntryCache();

  public: void updatePdoEntryValue(int pdoEntryId, PdoEntryValue pdoEntryValue);
  public: PdoEntryValue getPdoEntryValue(int pdoEntryId);
};
