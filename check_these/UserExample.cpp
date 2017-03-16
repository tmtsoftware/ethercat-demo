#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include "EtherCatServer.h"


int main() {

	// create a new server
	EtherCatServer etherCatServer = EtherCatServer();

	// configure it with generated XML file
	etherCatServer.configServer("/home/smichaels/git/ethercat-demo/test/test.xml");

	// print out device and parameter names
	std::vector<std::string> deviceNames = etherCatServer.getDeviceNames();
	for (unsigned int i=0; i<deviceNames.size(); i++) {
		std::cout << "\ndevice name " << i << ": " << deviceNames.at(i);

		std::vector<std::string> paramNames = etherCatServer.getParameterNames(deviceNames.at(i));
		for (unsigned int j=0; j<paramNames.size(); j++) {
			std::cout << "\n\tparameter name " << j << ": " << paramNames.at(j);
		}
	}

	std::cout << "\nabout to start server";

	etherCatServer.startServer();

	// this simulates the OPC/UA server loop
	int i;
	for (i=0; i<400; i++) {

		usleep(500000);

		std::cout << "\nServerSimulation:: setting parameter values";

		etherCatServer.setParameterValue("EL2202 2K. Dig. Ausgang 24V, 0.5A", "Channel 1::Output", i % 2);
		etherCatServer.setParameterValue("EL2202 2K. Dig. Ausgang 24V, 0.5A", "Channel 1::TriState", (i % 2) ? 0 : 1);

		etherCatServer.setParameterValue("EL2202 2K. Dig. Ausgang 24V, 0.5A", "Channel 2::Output", (i % 2) ? 0 : 1);
		etherCatServer.setParameterValue("EL2202 2K. Dig. Ausgang 24V, 0.5A", "Channel 2::TriState", i % 2);


		std::cout << "\nServerSimulation:: getting parameter values";

		// get some values from the server
		int val1 = etherCatServer.getParameterValue("EL2202 2K. Dig. Ausgang 24V, 0.5A", "Channel 1::Output");
		int val2 = etherCatServer.getParameterValue(0, 1,  "Channel 1::Input");
		std::cout << "\n\tChannel 1 Output = " << val1;
		std::cout << "\n\tDevice index 1,  Channel 1::Input = " << val2;

		std::cout << "\nWaited 500 ms, value = " << (i % 2) << "\n";
	}

	std::cout << "\nStopping Server";
	etherCatServer.stopServer();

	usleep(3000000);


	std::cout << "\nEnd";
}

/****************************************************************************/
