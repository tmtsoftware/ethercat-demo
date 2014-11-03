
class CyclicMotor {
 
private: ec_master_t *master;
private: ec_domain *domain1;
private: unsigned int *off_dig_out;
private: unsigned int *bp_dig_out;
private: uint8_t *domain1_pd;

public: CyclicMotor();

  public: CyclicMotor(ec_master_t *master, ec_domain *domain1, unsigned int *off_dig_out,
		  unsigned int *bp_dig_out, uint8_t *domain1_pd);

  public: void start();

  public: void startup();

  public: void stop();

  public: void cyclic_task(ec_master_t *master, ec_domain *domain1, unsigned int *off_dig_out,
		  unsigned int *bp_dig_out, uint8_t *domain1_pd);
};

