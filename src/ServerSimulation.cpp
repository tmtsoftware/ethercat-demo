#include <iostream>
#include "ecrt.h"
#include "TmtEcStructs.h"
#include "temp.h"
#include <string>
#include <vector>
#include <queue>
#include "PdoEntryCache.h"
#include "CommandQueue.h"
#include "CyclicMotor.h"
#include "ConfigLoader.h"
#include "EtherCatServer.h"

#include <unistd.h>

int main() {

	EtherCatServer etherCatServer = EtherCatServer();

	etherCatServer.configServer("TODO: config filename here");

	printf("about to start server");

	etherCatServer.startServer();



	// this simulates the OPC/UA server loop
	int i;
	for (i=0; i<400; i++) {

		usleep(500000);


		etherCatServer.setParameterValue("device1", "parameter1", i % 2);

		etherCatServer.setParameterValue("device1", "parameter2", (i % 2) ? 0 : 1);

		std::cout << "Waited 500 ms, value = " << (i % 2) << "\n";
	}

	std::cout << "Stopping Server";
	etherCatServer.stopServer();

	usleep(3000000);

	std::cout << "End";
}

/****************************************************************************/
