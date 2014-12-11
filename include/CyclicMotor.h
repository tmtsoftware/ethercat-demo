
#ifndef INCLUDE_CYCLICMOTOR_H_
#define INCLUDE_CYCLICMOTOR_H_

#include "ecrt.h"
#include <vector>
#include "SlaveConfig.h"


class CyclicMotor {
 
private: ec_master_t *master;
private: ec_domain *domain1;
private: uint8_t *domain1_pd;
private: std::vector<SlaveConfig> slaves;

public: CyclicMotor();

public: CyclicMotor(ec_master_t *master, ec_domain *domain1, unsigned int *off_dig_out,
		  unsigned int *bp_dig_out, uint8_t *domain1_pd);

public: CyclicMotor(ec_master_t *master, ec_domain *domain1, uint8_t *domain1_pd, std::vector<SlaveConfig> slaves);

  public: void start();

  public: void startup();

  public: void stop();

  public: void cyclic_task(ec_master_t *master, ec_domain *domain1, unsigned int *off_dig_out,
		  unsigned int *bp_dig_out, uint8_t *domain1_pd);
  public: void cyclic_task(ec_master_t *master, ec_domain *domain1, uint8_t *domain1_pd, std::vector<SlaveConfig> slaves);

};

#endif /* INCLUDE_CYCLICMOTOR_H_ */
