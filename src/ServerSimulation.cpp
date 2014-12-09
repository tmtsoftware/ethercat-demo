#include <iostream>
#include "ecrt.h"
#include "TmtEcStructs.h"
#include "temp.h"
#include "tinystr.h"
#include "tinyxml.h"
#include <string>
#include <vector>
#include <map>
#include <queue>
#include "PdoEntry.h"
#include "Pdo.h"
#include "SyncManager.h"
#include "SlaveConfig.h"
#include "PdoEntryCache.h"
#include "CommandQueue.h"
#include "CyclicMotor.h"
#include "ConfigLoader.h"
#include "EtherCatServer.h"

#include <unistd.h>

int main() {

	EtherCatServer etherCatServer = EtherCatServer();

	etherCatServer.configServer("/home/smichaels/git/ethercat-demo/test/test.xml");

	cout << "about to start server";


	etherCatServer.startServer();

	// this simulates the OPC/UA server loop
	int i;
	for (i=0; i<400; i++) {

		usleep(500000);

		cout << "\nServerSimulation:: setting parameter values";

		 etherCatServer.setParameterValue("EL2202 2K. Dig. Ausgang 24V, 0.5A", "Channel 1::Output", i % 2);
		 etherCatServer.setParameterValue("EL2202 2K. Dig. Ausgang 24V, 0.5A", "Channel 1::TriState", (i % 2) ? 0 : 1);

		 etherCatServer.setParameterValue("EL2202 2K. Dig. Ausgang 24V, 0.5A", "Channel 2::Output", (i % 2) ? 0 : 1);
		 etherCatServer.setParameterValue("EL2202 2K. Dig. Ausgang 24V, 0.5A", "Channel 2::TriState", i % 2);


		cout << "\nServerSimulation:: setting parameter values";

		std::cout << "\nWaited 500 ms, value = " << (i % 2) << "\n";
	}

	std::cout << "Stopping Server";
	etherCatServer.stopServer();

	usleep(3000000);


	std::cout << "End";
}

/****************************************************************************/
