#include <iostream>
#include <sstream>
#include <stdio.h>
#include "ecrt.h"
#include "tinystr.h"
#include "tinyxml.h"
#include "temp.h"
#include "TmtEcStructs.h"
#include <string>
#include <vector>

#include "PdoEntry.h"
#include "Pdo.h"
#include "SyncManager.h"
#include "SlaveConfig.h"
#include "ConfigLoader.h"

using namespace std;

ConfigLoader::ConfigLoader() {
}
;

void ConfigLoader::loadConfiguration(ec_master_t *master,
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

		// TEST ONLY
		ec_sync_info_t *sync = tmt_ec_slave[si].syncs;
		//printSyncs(1, sync);



		if (ecrt_slave_config_pdos(sc, 6, tmt_ec_slave[si].syncs)) {
			fprintf(stderr, "Failed to configure PDOs.\n");
			throw -1;
		}

		// PDO Entry Domain Registration

		ec_pdo_entry_info_t *pdo_entries = tmt_ec_slave[si].pdo_entries;
		for (int i = 0; i < tmt_ec_slave[si].pdo_entry_length; i++) {

			off_dig_out[i] = ecrt_slave_config_reg_pdo_entry(sc,
					pdo_entries[i].index, pdo_entries[i].subindex, domain1,
					&bp_dig_out[i]);
		}

	}
};

void ConfigLoader::loadConfiguration(ec_master_t *master, ec_domain *domain1, vector<SlaveConfig> *slaveConfigVector) {
	// configure all slaves and domain for each pdo entry of each slave

	cout << "\ngot here::" << slaveConfigVector->size() << "\n";

	ec_slave_config_t *sc;
	for (int si = 0; si < slaveConfigVector->size(); si++) {

		cout << "\nin loop: si = " << si << "\n";

		SlaveConfig slaveConfig = slaveConfigVector->at(si);

		// TEST ONLY
		vector<SyncManager> syncManagerVector = slaveConfig.syncs;
		// here we generate the structs
		ec_sync_info_t* syncs = new ec_sync_info_t[syncManagerVector.size()];

		cout << "syncs size = " << syncManagerVector.size();

		for (int i=0; i<syncManagerVector.size(); i++) {
			SyncManager syncManager = syncManagerVector.at(i);
			syncs[i].dir = syncManager.dir;
			syncs[i].index = syncManager.index;
			syncs[i].n_pdos = syncManager.pdos.size();
			syncs[i].watchdog_mode = syncManager.watchdog_mode;

			syncs[i].pdos = new ec_pdo_info_t[syncs[i].n_pdos];
			ec_pdo_info_t* pdos = syncs[i].pdos;

			for (int j=0; j<syncs[i].n_pdos; j++) {

				Pdo pdo = syncManager.pdos.at(j);

				syncs[i].pdos[j].index = pdo.index;
				syncs[i].pdos[j].n_entries = pdo.n_entries;

				pdos[j].entries = new ec_pdo_entry_info_t[syncs[i].pdos[j].n_entries];
				ec_pdo_entry_info_t* pdo_entries = pdos[j].entries;

				for (int k=0; k<pdos[j].n_entries; k++) {

					PdoEntry pdoEntry = pdo.pdoEntries.at(k);

					pdo_entries[k].index = pdoEntry.index;
					pdo_entries[k].subindex = pdoEntry.subindex;
					pdo_entries[k].bit_length = pdoEntry.bitLength;
				}

			}
		}

		printSyncs(syncManagerVector.size(), syncs);

		cout << hex << slaveConfig.alias << "::" << slaveConfig.pos << "::" << slaveConfig.vendor_id << "::" << slaveConfig.product_code;


		// Create configuration for bus coupler
		sc = ecrt_master_slave_config(master, slaveConfig.alias,
				slaveConfig.pos, slaveConfig.vendor_id,
				slaveConfig.product_code);

		if (!sc)
			throw -1;

		printf("Configuring PDOs...\n");

		if (ecrt_slave_config_pdos(sc, syncManagerVector.size(), syncs)) {
			fprintf(stderr, "Failed to configure PDOs.\n");
			throw -1;
		}

		// PDO Entry Domain Registration

		for (int i = 0; i < slaveConfig.pdoEntries.size(); i++) {

			PdoEntry pdoEntry = slaveConfig.pdoEntries.at(i);

			unsigned int offset = 0;
			unsigned int bitpos = 0;
			offset = ecrt_slave_config_reg_pdo_entry(sc,
					pdoEntry.index, pdoEntry.subindex, domain1,
					&bitpos);

			slaveConfigVector->at(si).pdoEntries.at(i).domainOffset = offset;
			slaveConfigVector->at(si).pdoEntries.at(i).domainBitPos = bitpos;

			cout << hex << "\n" << slaveConfigVector->at(si).pdoEntries.at(i).domainOffset << "<>" << slaveConfigVector->at(si).pdoEntries.at(i).domainBitPos;

		}



	}
	printf("\n");
}


vector<SlaveConfig> ConfigLoader::loadConfiguration(string configFile) {

	// Decision: there is no public API for getting PDO entries, etc.  This information *could* be obtained using the source
	// from the 'tool' area, but this appears to be fairly complex and would be time consuming.  The information we want is still
	// available from the 'tool' commands, and to support new versions, it is probably cleaner to just use the command line tools
	// as is and use the tool outputs directly, rather than trying to recreate the tools in other code.

	vector<SlaveConfig> slaveConfigList;

	TiXmlNode* pChild;

	TiXmlDocument doc(configFile.c_str());
	doc.LoadFile();
	printf("\n%s:\n", configFile.c_str());

	TiXmlNode* root = doc.FirstChild("EtherCATInfoList");

	// initialize array with element for each slave
	tmt_ec_slave_t* tmt_ec_slave = new tmt_ec_slave_t[countChildren(root)];

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

		tmt_ec_slave[slaveIndex].name = new string(nameText->Value());

		tmt_ec_slave[slaveIndex].product_code = hexCharToInt(typeElem->Attribute("ProductCode"));
		tmt_ec_slave[slaveIndex].pos = slaveIndex;
		tmt_ec_slave[slaveIndex].vendor_id = charToInt(idValueText->Value());
		tmt_ec_slave[slaveIndex].alias = 0; // FIXME: this part requires another tool to set up

		// loop over a slave's Sync Managers
		// the order is important in linking them to the PDOs

		// also need to create this for the ecrt structs
		ec_sync_info_t* ecrt_syncs = new ec_sync_info_t[countChildren(deviceElem, "Sm")];

		// add both sync types to slave structure
		tmt_ec_slave[slaveIndex].syncs = ecrt_syncs;

		SlaveConfig *slaveConfig = new SlaveConfig(*(tmt_ec_slave[slaveIndex].name), tmt_ec_slave[slaveIndex].vendor_id,
				tmt_ec_slave[slaveIndex].product_code, tmt_ec_slave[slaveIndex].alias, tmt_ec_slave[slaveIndex].pos);

		vector<Pdo> slavePdoVector;
		vector<PdoEntry> slavePdoEntryVector;
		vector<SyncManager> slaveSyncManagerVector;

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

			ec_direction_t dir = (txCount > 0) ? EC_DIR_INPUT : EC_DIR_OUTPUT;
			uint8_t index = i;
			unsigned int n_pdos = (txCount > 0) ? txCount : rxCount;
			ec_watchdog_mode_t watchdog_mode = (txCount > 0) ? EC_WD_DISABLE : EC_WD_ENABLE;

			// duplicate values into ecrt sync structure
			ecrt_syncs[i].dir = dir;
			ecrt_syncs[i].index = index;
			ecrt_syncs[i].n_pdos = n_pdos;
			ecrt_syncs[i].watchdog_mode = watchdog_mode;

			vector<Pdo> pdos;
			if (txCount > 0) {
				pdos = loadPdoInfo("TxPdo", deviceElem, i, &ecrt_syncs[i]);
			} else {
				pdos = loadPdoInfo("RxPdo", deviceElem, i, &ecrt_syncs[i]);
			}

			SyncManager *syncManager = new SyncManager(index, dir, watchdog_mode);

			// add to running vectors of pdos and pdo entries for this slave
			for(int pdoi = 0; pdoi < pdos.size(); pdoi++) {
			    Pdo pdo = pdos.at(pdoi);

			    // add to sync manager
			    syncManager->pdos.push_back(pdo);

			    // add to slave totals
			    slaveConfig->pdos.push_back(pdo);

			    cout << "\n&&&& " << pdo.name << ", size = " << pdo.pdoEntries.size();

			    for(int pdoei = 0; pdoei < pdo.pdoEntries.size(); pdoei++) {

			    	PdoEntry pdoEntry = pdo.pdoEntries.at(pdoei);

			    	slaveConfig->pdoEntries.push_back(pdoEntry);

			    	cout << pdoEntry.entryName << ", ";

			    }

			}
			cout << "\n";


			slaveSyncManagerVector.push_back(*syncManager);
		}

		// add completed sync tree structs to the slave config
		slaveConfig->syncs = slaveSyncManagerVector;



		slaveConfigList.push_back(*slaveConfig);
	}


	return slaveConfigList;

}

vector<Pdo> ConfigLoader::loadPdoInfo(const char* type, TiXmlElement *deviceElem,
		int smIndex, ec_sync_info_t* ecrt_sync) {

// allocate the tmt pdos array - new

	vector<Pdo> pdo_vector;

	tmt_ec_pdo* slave_pdos = new tmt_ec_pdo[ecrt_sync->n_pdos];
	ec_pdo_info_t* ecrt_pdos = new ec_pdo_info_t[ecrt_sync->n_pdos];

	cout << "\nnumber of pdos " << ecrt_sync->n_pdos << "\n";

	if (ecrt_sync->n_pdos == 0) {
		return pdo_vector;
	}

// add to sync manager structure
	ecrt_sync->pdos = ecrt_pdos;

// loop over a slave's PDOs
	TiXmlNode* pdoChild = 0;
	int j = 0;
	for (pdoChild = deviceElem->FirstChild(type); pdoChild != 0; pdoChild = pdoChild->NextSibling()) {

		cout << "\n\nPdo " << j++ << "\n";

		//tmt_ec_pdo slave_pdo = slave_pdos[j];

		ec_pdo_info_t ecrt_pdo = ecrt_pdos[j];

		TiXmlElement* pdoElem = pdoChild->ToElement();

		int syncMgr = charToInt(pdoElem->Attribute("Sm"));
		if (syncMgr != smIndex) continue;


		(ecrt_sync->n_pdos)++;  // increment the number of pdos for this sync mgr

		cout << "\n Sync Manager = " << pdoElem->Attribute("Sm");
		cout << "\n fixed = " << pdoElem->Attribute("Fixed");
		cout << "\n mandatory = " << pdoElem->Attribute("Mandatory");



		TiXmlText* indexText = pdoElem->FirstChild("Index")->FirstChild()->ToText();

		cout << "\n index = " << indexText->Value();

		unsigned int index = hexCharToInt(indexText->Value());

		cout << "    >>>>>> 0x" << hex << index;

		string *name = new string("Not Specified");

		TiXmlText* nameText = pdoElem->FirstChild("Name")->FirstChild()->ToText();
		cout << "\n name = " << nameText->Value();
		name = new string(nameText->Value());


		int pdoEntryCount = countChildren(pdoElem, "Entry");
		int n_entries = pdoEntryCount;

		// copy values to ecrt structure
		ecrt_pdo.index = index;
		ecrt_pdo.n_entries = pdoEntryCount;

		// allocate tmt and ecrt pdo entries structures
		tmt_ec_pdo_entry* pdo_entries = new tmt_ec_pdo_entry[pdoEntryCount];
		ec_pdo_entry_info_t* ecrt_pdo_entries = new ec_pdo_entry_info_t[pdoEntryCount];

		//pdo->pdo_entries = pdo_entries;
		ecrt_pdo.entries = ecrt_pdo_entries;

		// save this pdo
		Pdo *pdo = new Pdo(*name, index, n_entries);
		pdo_vector.push_back(*pdo);



		// loop over all PDO entries


		TiXmlNode* pdoEntryChild = 0;
		int k = 0;
		for (pdoEntryChild = pdoElem->FirstChild("Entry"); pdoEntryChild != 0; pdoEntryChild = pdoEntryChild->NextSibling()) {


			cout << "\n-----------------\n";
			cout << "Pdo Entry " << k++ << "\n";

			//tmt_ec_pdo_entry pdo_entry = pdo_entries[k]; // tmt struct
			ec_pdo_entry_info_t ecrt_pdo_entry = ecrt_pdo_entries[k];  // ecrt struct


			TiXmlText* indexText = pdoEntryChild->FirstChild("Index")->FirstChild()->ToText();
			cout << "\n index = " << indexText->Value();

			TiXmlText* bitLenText = pdoEntryChild->FirstChild("BitLen")->FirstChild()->ToText();
			cout << "\n bit length = " << bitLenText->Value();

			int bit_length = charToInt(bitLenText->Value());
			unsigned int index = hexCharToInt(indexText->Value());

			int subindex = 0;

			if (pdoEntryChild->FirstChild("SubIndex")) {

				TiXmlText* subIndexText = pdoEntryChild->FirstChild("SubIndex")->FirstChild()->ToText();
				cout << "\n sub-index = " << subIndexText->Value();
				subindex = charToInt(subIndexText->Value());

			} else {
				cout << "\n sub-index = " << "0x00";
			}

			string *entry_name = new string("Not Specified");

			if (pdoEntryChild->FirstChild("Name")) {

				TiXmlText* nameText = pdoEntryChild->FirstChild("Name")->FirstChild()->ToText();
				cout << "\n name = " << nameText->Value();
				entry_name = new string(nameText->Value());
			} else {
				cout << "\n name = " << "Not Specified " << k;
			}


			if (pdoEntryChild->FirstChild("DataType")) {
				TiXmlText* dataTypeText = pdoEntryChild->FirstChild("DataType")->FirstChild()->ToText();
				cout << "\n data type = " << dataTypeText->Value();
			} else {
				cout << "\n data type = " << "None";
			}

			// fill the ecrt structure
			ecrt_pdo_entry.index = index;
			ecrt_pdo_entry.subindex = subindex;
			ecrt_pdo_entry.bit_length = bit_length;


			// save this as pdoEntry
			PdoEntry *pdoEntry = new PdoEntry(*entry_name, pdo->name, index, subindex, bit_length);
			pdo_vector.back().pdoEntries.push_back(*pdoEntry);

			cout << "\n---------" << pdo_vector.size() << "--------\n";

		}
		cout << "all PDO entries done";

	}

	return pdo_vector;

}

unsigned int ConfigLoader::charToInt(const char* input) {

	unsigned int intValue;

	intValue = strtol(input, NULL, 0);

	return intValue;
}

unsigned int ConfigLoader::hexCharToInt(const char* input) {

	unsigned int intValue;

	string *sindex = new string(input);

	sindex->replace(0, 1, "0");

	stringstream strValue;
	strValue << *sindex;

	strValue >> hex >> intValue;

	return intValue;
}



int ConfigLoader::countChildren(TiXmlNode* parent) {

	//cout << "countChildren1";
	int childCount = 0;
	TiXmlNode* child;
	for (child = parent->FirstChild(); child; child = child->NextSibling()) {
		childCount++;
	}
	return childCount;
}

int ConfigLoader::countChildren(TiXmlNode* parent, const char* name) {
	//cout << "countChildren2";
	int childCount = 0;
	TiXmlNode* child;
	for (child = parent->FirstChild(name); child; child =
			child->NextSibling()) {
		childCount++;
	}
	return childCount;
}

int ConfigLoader::countPdoChildren(TiXmlNode* parent, const char* name, int smIndex) {
	//cout << "countPdoChildren";
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

void ConfigLoader::printSyncs(int n_syncs, ec_sync_info_t *sync) {

	cout << "in printSyncs nsyncs = " << n_syncs;

	for (int i=0; i<n_syncs; i++) {

		printf("\nnsync = %hx :: %hx :: %hx :: %hx", sync[i].dir, sync[i].index, sync[i].n_pdos, sync[i].watchdog_mode);


		for (int j=0; j<sync[i].n_pdos; j++) {

			ec_pdo_info_t *pdos = sync[i].pdos;
			printf("\n\tpdo = %hx :: %hx", pdos[j].index, pdos[j].n_entries);

			for (int k=0; k<pdos[j].n_entries; k++) {
				ec_pdo_entry_info_t *pdo_entry = pdos[j].entries;
				printf("\n\t\tentry = %hx :: %hx :: %hx", pdo_entry[k].index, pdo_entry[k].subindex, pdo_entry[k].bit_length);
			}
		}
	}

	printf("\n");
}
