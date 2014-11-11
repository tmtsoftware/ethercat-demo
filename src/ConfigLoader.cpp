#include <iostream>
#include <stdio.h>
#include "ecrt.h"
#include "tinystr.h"
#include "tinyxml.h"
#include "temp.h"
#include "TmtEcStructs.h"
#include <vector>

#include "ConfigLoader.h"

using namespace std;

ConfigLoader::ConfigLoader() {
}
;

vector<SlaveConfig> ConfigLoader::loadConfiguration(ec_master_t *master,
		tmt_ec_slave_t *tmt_ec_slave, ec_domain *domain1,
		unsigned int *off_dig_out, unsigned int *bp_dig_out) {

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

vector<SlaveConfig> ConfigLoader::loadConfiguration(string configFile) {

	// Decision: there is no public API for getting PDO entries, etc.  This information *could* be obtained using the source
	// from the 'tool' area, but this appears to be fairly complex and would be time consuming.  The information we want is still
	// available from the 'tool' commands, and to support new versions, it is probably cleaner to just use the command line tools
	// as is and use the tool outputs directly, rather than trying to recreate the tools in other code.

	TiXmlNode* pChild;

	TiXmlDocument doc(configFile.c_str());
	doc.LoadFile();
	printf("\n%s:\n", configFile.c_str());

	TiXmlNode* root = doc.FirstChild("EtherCATInfoList");

	// initialize array with element for each slave
	tmt_ec_slave_t tmt_ec_slave[countChildren(root)];

	// loop over slaves
	int slaveIndex = -1;
	for (pChild = root->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {

		slaveIndex++;
		cout << "\n child value = " << pChild->Value();

		// Slave Information

		TiXmlElement* vendorElem = pChild->FirstChildElement("Vendor");
		TiXmlElement* idElem = vendorElem->FirstChildElement("Id");
		TiXmlText* idValueText = idElem->FirstChild()->ToText();

		cout << "\n vendor id: " << idValueText->Value();

		TiXmlElement* descriptionsElem = pChild->FirstChildElement("Descriptions");
		TiXmlElement* devicesElem = descriptionsElem->FirstChildElement("Devices");
		TiXmlElement* deviceElem = devicesElem->FirstChildElement("Device");
		TiXmlElement* typeElem = deviceElem->FirstChildElement("Type");

		TiXmlText* typeValueText = typeElem->FirstChild()->ToText();

		cout << "\n device type = " << typeValueText->Value();

		// product code and revision number

		cout << "\n product code = " << typeElem->Attribute("ProductCode");
		cout << "\n revision # = " << typeElem->Attribute("RevisionNo");

		// device name

		TiXmlElement* nameElem = deviceElem->FirstChildElement("Name");
		TiXmlText* nameText = nameElem->FirstChild()->ToText();

		cout << "\n device name = " << nameText->Value();

		// fill the structure with parsed values
		tmt_ec_slave[slaveIndex].name = nameText->Value();
		tmt_ec_slave[slaveIndex].product_code = charToInt(typeElem->Attribute("ProductCode"));
		tmt_ec_slave[slaveIndex].pos = 0;
		tmt_ec_slave[slaveIndex].vendor_id = charToInt(idValueText->Value());
		tmt_ec_slave[slaveIndex].alias = 1111; // FIXME: this part requires another tool to set up

		// loop over a slave's Sync Managers
		// the order is important in linking them to the PDOs

		ec_sync_info_t slave_syncs[countChildren(deviceElem, "Sm")];

		TiXmlNode* smChild = 0;
		int i = -1;
		while ((smChild = deviceElem->IterateChildren("Sm", smChild))) {

			cout << "\n\nSync Manager " << ++i << "\n";

			TiXmlElement* smElem = smChild->ToElement();

			cout << "\n start address = " << smElem->Attribute("StartAddress");
			cout << "\n enable = " << smElem->Attribute("Enable");
			cout << "\n control byte = " << smElem->Attribute("ControlByte");
			cout << "\n default size = " << smElem->Attribute("DefaultSize");

			//  we must only be counting those that are for Sync manager #i
			int txCount = countPdoChildren(deviceElem, "TxPdo", i);
			int rxCount = countPdoChildren(deviceElem, "RxPdo", i);

			slave_syncs[i].dir = (txCount > 0) ? EC_DIR_INPUT : EC_DIR_OUTPUT;
			slave_syncs[i].index = i;
			slave_syncs[i].n_pdos = (txCount > 0) ? txCount : rxCount;
			slave_syncs[i].watchdog_mode = (txCount > 0) ? EC_WD_DISABLE : EC_WD_ENABLE;

			if (txCount > 0) {
				loadPdoInfo("TxPdo", deviceElem, i, slave_syncs[i]);
			} else {
				loadPdoInfo("RxPdo", deviceElem, i, slave_syncs[i]);
			}

		}

	}

	// satisfy interface
	SlaveConfig slaveConfig;

	vector<SlaveConfig> slaveConfigList;

	slaveConfigList.push_back(slaveConfig);

	return slaveConfigList;

}

void ConfigLoader::loadPdoInfo(const char* type, TiXmlElement *deviceElem,
		int smIndex, ec_sync_info_t slave_sync) {

// allocate slave pdos array
	ec_pdo_info_t slave_pdos[slave_sync.n_pdos];

// add to sync manager structure
	slave_sync.pdos = slave_pdos;

// loop over a slave's PDOs
	TiXmlNode* pdoChild = 0;
	int j = 0;
	for (pdoChild = deviceElem->FirstChild(type); pdoChild != 0; pdoChild = pdoChild->NextSibling()) {

		cout << "\n\nPdo " << j++ << "\n";

		TiXmlElement* pdoElem = pdoChild->ToElement();

		cout << "\n Sync Manager = " << pdoElem->Attribute("Sm");
		cout << "\n fixed = " << pdoElem->Attribute("Fixed");
		cout << "\n mandatory = " << pdoElem->Attribute("Mandatory");

		TiXmlText* indexText = pdoElem->FirstChild("Index")->FirstChild()->ToText();

		cout << "\n index = " << indexText->Value();

		TiXmlText* nameText = pdoElem->FirstChild("Name")->FirstChild()->ToText();

		cout << "\n name = " << nameText->Value();

		// fill up the slave_pdos struct
		slave_pdos[j].index = charToInt(indexText->Value());

		int pdoEntryCount = countChildren(pdoElem, "Entry");
		slave_pdos[j].n_entries = pdoEntryCount;
		ec_pdo_entry_info_t slave_pdo_entries[pdoEntryCount];
		slave_pdos[j].entries = slave_pdo_entries;

		// loop over all PDO entries

		TiXmlNode* pdoEntryChild = 0;
		int k = 0;
		for (pdoEntryChild = pdoElem->FirstChild("Entry"); pdoEntryChild != 0; pdoEntryChild = pdoEntryChild->NextSibling()) {


			cout << "\n-----------------\n";
			cout << "Pdo Entry " << k++ << "\n";

			TiXmlText* indexText = pdoEntryChild->FirstChild("Index")->FirstChild()->ToText();
			cout << "\n index = " << indexText->Value();

			TiXmlText* bitLenText = pdoEntryChild->FirstChild("BitLen")->FirstChild()->ToText();
			cout << "\n bit length = " << bitLenText->Value();

			slave_pdo_entries[k].bit_length = charToInt(bitLenText->Value());
			slave_pdo_entries[k].index = charToInt(indexText->Value());

			if (pdoEntryChild->FirstChild("SubIndex")) {

				TiXmlText* subIndexText = pdoEntryChild->FirstChild("SubIndex")->FirstChild()->ToText();
				cout << "\n sub-index = " << subIndexText->Value();
				slave_pdo_entries[k].subindex = charToInt(subIndexText->Value());

			} else {
				cout << "\n sub-index = " << "0x00";
				slave_pdo_entries[k].subindex = 0;
			}

			if (pdoEntryChild->FirstChild("Name")) {

				TiXmlText* nameText = pdoEntryChild->FirstChild("Name")->FirstChild()->ToText();
				cout << "\n name = " << nameText->Value();
			} else {
				cout << "\n name = " << "Not Specified " << k;
			}

			if (pdoEntryChild->FirstChild("DataType")) {
				TiXmlText* dataTypeText = pdoEntryChild->FirstChild("DataType")->FirstChild()->ToText();
				cout << "\n data type = " << dataTypeText->Value();
			} else {
				cout << "\n data type = " << "None";
			}

			cout << "\n-----------------\n";

		}
		cout << "all PDO entries done";


	}
}

unsigned int ConfigLoader::charToInt(const char* input) {

	unsigned int intValue;

	intValue = strtol(input, NULL, 0);

	return intValue;
}

int ConfigLoader::countChildren(TiXmlNode* parent) {

	cout << "countChildren1";
	int childCount = 0;
	TiXmlNode* child;
	for (child = parent->FirstChild(); child; child = child->NextSibling()) {
		childCount++;
	}
	return childCount;
}

int ConfigLoader::countChildren(TiXmlNode* parent, const char* name) {
	cout << "countChildren2";
	int childCount = 0;
	TiXmlNode* child;
	for (child = parent->FirstChild(name); child; child =
			child->NextSibling()) {
		childCount++;
	}
	return childCount;
}

int ConfigLoader::countPdoChildren(TiXmlNode* parent, const char* name, int smIndex) {
	cout << "countPdoChildren";
	int childCount = 0;
	TiXmlNode* child;
	for (child = parent->FirstChild(name); child; child = child->NextSibling()) {
		TiXmlElement *childElement = child->ToElement();
		int candidateIndex = charToInt(childElement->Attribute("Sm"));
		if (candidateIndex == smIndex) {
			childCount++;
		}
	}
	return childCount;
}
