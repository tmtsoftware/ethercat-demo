#include <iostream>
#include <stdio.h>
#include "ecrt.h"
#include "TmtEcStructs.h"
#include "temp.h"
#include "CyclicMotor.h"

#include <thread>

using namespace std;

static unsigned int counter = 0;
static unsigned int blink = 0;
static bool terminateFlg = false;

// cyclic motor state

CyclicMotor::CyclicMotor() {};

CyclicMotor::CyclicMotor(ec_master_t *master, ec_domain *domain1, unsigned int *off_dig_out,
		  unsigned int *bp_dig_out, uint8_t *domain1_pd) {

	this->master = master;
	this->domain1 = domain1;
	this->off_dig_out = off_dig_out;
	this->bp_dig_out = bp_dig_out;
	this->domain1_pd = domain1_pd;

};

void CyclicMotor::start() {

	terminateFlg = false;

	std::thread t1(&CyclicMotor::startup, this);
	t1.detach();

};

void CyclicMotor::stop() {
	terminateFlg = true;
};

#if 0
/*****************************************************************************/

void check_domain1_state(void)
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

void CyclicMotor::startup()
{

// everything below here is eventually in cyclic motor

    printf("Started.\n");



    while (1) {
        auto now = std::chrono::system_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

#if 1
        struct timeval t;
        gettimeofday(&t, NULL);
        printf("%u.%03u\n", t.tv_sec, t.tv_usec);
#endif

      	this->cyclic_task(this->master, this->domain1, this->off_dig_out, this->bp_dig_out, this->domain1_pd);
    }
};

/*****************************************************************************/

void CyclicMotor::cyclic_task(ec_master_t *master, ec_domain *domain1, unsigned int *off_dig_out,
		  unsigned int *bp_dig_out, uint8_t *domain1_pd) {

	// receive process data
	ecrt_master_receive(master);
	ecrt_domain_process(domain1);

	// check process data state (optional)
	//check_domain1_state();

	if (counter) {
		counter--;
	} else { // do this at 1 Hz

		counter = FREQUENCY / 10;
		// calculate new process data
		blink = !blink;

		// check for master state (optional)
		//check_master_state();

		// check for islave configuration state(s) (optional)

		//check_slave_config_states();


		if (terminateFlg) {
			std::cout << "terminating thread";
			std::terminate();
			return;
		}


		int si = 0;
		for (si = 0; si < 1; si++) {
			/*
			 int i;
			 for (i=0; i<tmt_ec_slave[si].pdo_entry_length; i++) {
			 // this is where the case statements will be depending on field length of the PDO entry
			 EC_WRITE_BIT(domain1_pd + tmt_ec_slave[si].domain_offset[i], tmt_ec_slave[si].domain_bit_pos[i], blink ? 0x1 : 0x0);
			 }
			 */


			EC_WRITE_BIT(domain1_pd + off_dig_out[0], bp_dig_out[0],
					blink ? 0x1 : 0x0);
			EC_WRITE_BIT(domain1_pd + off_dig_out[1], bp_dig_out[1],
					blink ? 0x0 : 0x1);
			EC_WRITE_BIT(domain1_pd + off_dig_out[2], bp_dig_out[2],
					blink ? 0x0 : 0x1);
			EC_WRITE_BIT(domain1_pd + off_dig_out[3], bp_dig_out[3],
					blink ? 0x1 : 0x0);

		}
	}


	// send process data
	ecrt_domain_queue(domain1);
	ecrt_master_send(master);
};

