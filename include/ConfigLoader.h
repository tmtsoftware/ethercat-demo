
using namespace std;

class ConfigLoader {
 
  public: ConfigLoader();
  public: void loadConfiguration(ec_master_t *master, tmt_ec_slave_t *tmt_ec_slave, ec_domain *domain1,
		  unsigned int *off_dig_out, unsigned int *bp_dig_out);

  public: void loadConfiguration(ec_master_t *master, ec_domain *domain1, vector<SlaveConfig> *slaveConfigVector);

  public: vector<SlaveConfig> loadConfiguration(string configFile);

  private: vector<Pdo> loadPdoInfo(const char* type, TiXmlElement *deviceElem, int smIndex, ec_sync_info_t* slave_sync);

  private: unsigned int charToInt(const char* input);
  private: unsigned int hexCharToInt(const char* input);

  private: int countChildren(TiXmlNode* parent);

  private: int countChildren(TiXmlNode* parent, const char* name);
  private: int countPdoChildren(TiXmlNode* parent, const char* name, int smIndex);

  private: void printSyncs(int nSyncs, ec_sync_info_t *sync);
};
