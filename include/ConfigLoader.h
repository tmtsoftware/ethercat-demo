
using namespace std;

class ConfigLoader {
 
  public: ConfigLoader();
  public: vector<SlaveConfig> loadConfiguration(ec_master_t *master, tmt_ec_slave_t *tmt_ec_slave, ec_domain *domain1, unsigned int *off_dig_out,
		  unsigned int *bp_dig_out);
};

