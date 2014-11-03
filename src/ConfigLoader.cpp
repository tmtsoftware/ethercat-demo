#include <iostream>
#include <stdio.h>
#include "ecrt.h"
#include "temp.h"
#include "TmtEcStructs.h"
#include <vector>
#include "ConfigLoader.h"
using namespace std;

ConfigLoader::ConfigLoader() {
}
;

vector<SlaveConfig> ConfigLoader::loadConfiguration(ec_master_t *master, tmt_ec_slave_t *tmt_ec_slave, ec_domain *domain1, unsigned int *off_dig_out,
		  unsigned int *bp_dig_out) {

	// configure all slaves and domain for each pdo entry of each slave
	ec_slave_config_t *sc;

	int si = 0;
	for (si = 0; si < 1; si++) {

		// Create configuration for bus coupler
		sc = ecrt_master_slave_config(master, tmt_ec_slave[si].alias,
				tmt_ec_slave[si].pos, tmt_ec_slave[si].vendor_id,
				tmt_ec_slave[si].product_code);

		if (!sc)
			throw -1;

		printf("Configuring PDOs...\n");

		if (ecrt_slave_config_pdos(sc, 6, tmt_ec_slave[si].syncs)) {
			fprintf(stderr, "Failed to configure PDOs.\n");
			throw -1;
		}

		// PDO Entry Domain Registration
		int i;
		ec_pdo_entry_info_t *pdo_entries = tmt_ec_slave[si].pdo_entries;
		for (i = 0; i < tmt_ec_slave[si].pdo_entry_length; i++) {

			off_dig_out[i] = ecrt_slave_config_reg_pdo_entry(sc,
					pdo_entries[i].index, pdo_entries[i].subindex, domain1,
					&bp_dig_out[i]);

			//tmt_ec_slave[si].domain_offset[i] = ecrt_slave_config_reg_pdo_entry(sc, pdo_entries[i].index,
			//    pdo_entries[i].subindex, domain1, &(tmt_ec_slave[si].domain_bit_pos[i]));
		}

	}

	SlaveConfig slaveConfig;

	vector<SlaveConfig> slaveConfigList;

	slaveConfigList.push_back(slaveConfig);

	return slaveConfigList;

}
;

