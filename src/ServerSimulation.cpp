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
	for (i=0; i<200; i++) {

		usleep(1000000);
		std::cout << "Waited 1000 ms\n";
	}

	std::cout << "Stopping Server";
	etherCatServer.stopServer();

	usleep(3000000);

	std::cout << "End";
}

/****************************************************************************/
