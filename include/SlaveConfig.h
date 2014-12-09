using namespace std;

class SlaveConfig {
 
  public: SlaveConfig(string name, unsigned int vendor_id, unsigned int product_code, unsigned int alias, unsigned int pos);

  public: string name;
  public: unsigned int vendor_id;
  public: unsigned int product_code;
  public: unsigned int alias;
  public: unsigned int pos;
  public: unsigned int *domain_offset;
  public: unsigned int *domain_bit_pos;
  public: vector<SyncManager> syncs;  	    // full tree for PDO Configuration
  public: vector<PdoEntry> pdoEntries;
  public: vector<Pdo> pdos;

};

