#include <iostream>
#include <stdio.h>
#include "ecrt.h"

#include <vector>
#include "PdoEntry.h"
#include "Pdo.h"
#include "SyncManager.h"
#include "SlaveConfig.h"
#include "TmtEcStructs.h"
#include "temp.h"
#include "CommandQueue.h"
#include "PdoEntryCache.h"
#include "CyclicMotor.h"


#include <thread>

static bool terminateFlg = false;
static ec_domain_state_t domain1_state = {};


// cyclic motor state

CyclicMotor::CyclicMotor() {
};

CyclicMotor::CyclicMotor(ec_master_t *master, ec_domain *domain1, uint8_t *domain1_pd, std::vector<SlaveConfig> slaves) {
	this->master = master;
	this->domain1 = domain1;
	this->domain1_pd = domain1_pd;
	this->slaves = slaves;
};


void CyclicMotor::start() {

	terminateFlg = false;

	std::thread t1(&CyclicMotor::startup, this);
	t1.detach();

};

void CyclicMotor::stop() {
	terminateFlg = true;
};


/*****************************************************************************/

void check_domain1_state(ec_domain *domain1)
{
	ec_domain_state_t ds;

	ecrt_domain_state(domain1, &ds);

	if (ds.working_counter != domain1_state.working_counter)
	printf("Domain1: WC %u.\n", ds.working_counter);
	if (ds.wc_state != domain1_state.wc_state)
	printf("Domain1: State %u.\n", ds.wc_state);

	domain1_state = ds;
}

/*****************************************************************************/
#if 0
void check_master_state(void)
{
	ec_master_state_t ms;

	ecrt_master_state(master, &ms);

	if (ms.slaves_responding != master_state.slaves_responding) {
		printf("%u slave(s).\n", ms.slaves_responding);
	}
	if (ms.al_states != master_state.al_states)
	printf("AL states: 0x%02X.\n", ms.al_states);
	if (ms.link_up != master_state.link_up)
	printf("Link is %s.\n", ms.link_up ? "up" : "down");

	master_state = ms;
}

/*****************************************************************************/

void check_slave_config_states(void)
{
	ec_slave_config_state_t s;

	ecrt_slave_config_state(sc_ana_in, &s);

	if (s.al_state != sc_ana_in_state.al_state)
	printf("AnaIn: State 0x%02X.\n", s.al_state);
	if (s.online != sc_ana_in_state.online)
	printf("AnaIn: %s.\n", s.online ? "online" : "offline");
	if (s.operational != sc_ana_in_state.operational)
	printf("AnaIn: %soperational.\n",
			s.operational ? "" : "Not ");

	sc_ana_in_state = s;
}

#endif

void CyclicMotor::startup() {

	printf("Started.\n");

	while (1) {
		auto now = std::chrono::system_clock::now();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

#if 0
		struct timeval t;
		gettimeofday(&t, NULL);
		printf("%u.%03u\n", t.tv_sec, t.tv_usec);
#endif

		this->cyclic_task(this->master, this->domain1, this->domain1_pd, this->slaves);
	}
};



void CyclicMotor::cyclic_task(ec_master_t *master, ec_domain *domain1, uint8_t *domain1_pd, std::vector<SlaveConfig> slaves) {

	//cout << "cyclic_task::\n";

	// receive process data
	ecrt_master_receive(master);
	ecrt_domain_process(domain1);

	// check process data state (optional)
	check_domain1_state(domain1);

	// check for master state (optional)
	//check_master_state();

	// check for islave configuration state(s) (optional)

	//check_slave_config_states();

	if (terminateFlg) {
		std::cout << "\nterminating thread";
		std::terminate();
		return;
	}

	// read all values into the cache
	// TODO: this could be in a separate thread

	for (unsigned int si=0; si<slaves.size(); si++) {
		SlaveConfig slaveConfig = slaves.at(si);
		for (unsigned int i=0; i<slaveConfig.pdoEntries.size(); i++) {
			PdoEntry pdoEntry = slaveConfig.pdoEntries.at(i);

			// only fill the hash if there is no entry
			int value = EC_READ_BIT(domain1_pd + pdoEntry.domainOffset, pdoEntry.domainBitPos);
			//std::cout << "\nreading value = " << value << ", " << "offset = " << pdoEntry.domainOffset << ", bitpos = " << pdoEntry.domainBitPos;
			//std::cout << ", slave = " << si << ", index = " << i;

			// put in cache
			PdoEntryCache *cache = PdoEntryCache::instance();
			cache->updatePdoEntryValue(si, i, value);
		}
	}

	// write all values in the queue
	while (!CommandQueue::instance()->isEmpty()) {

		PdoEntryValue pdoEntryValue = CommandQueue::instance()->getNext();

		int index = pdoEntryValue.pdoEntryIndex;
		int si = pdoEntryValue.slaveIndex;
		SlaveConfig slaveConfig = slaves.at(si);

		PdoEntry pdoEntry = slaveConfig.pdoEntries.at(index);

		std::cout << "\nwriting value = " << pdoEntryValue.entryValue << ", " << "offset = " << pdoEntry.domainOffset << ", bitpos = " << pdoEntry.domainBitPos;

		EC_WRITE_BIT(domain1_pd + pdoEntry.domainOffset, pdoEntry.domainBitPos, pdoEntryValue.entryValue);

	}

	// send process data
	ecrt_domain_queue(domain1);
	ecrt_master_send(master);
};
