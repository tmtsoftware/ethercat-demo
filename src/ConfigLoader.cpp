#include <iostream>
#include <sstream>
#include <stdio.h>
#include "ecrt.h"
#include "tinystr.h"
#include "tinyxml.h"
#include "temp.h"
#include <string>
#include <vector>

#include "PdoEntry.h"
#include "Pdo.h"
#include "SyncManager.h"
#include "SlaveConfig.h"
#include "ConfigLoader.h"


ConfigLoader::ConfigLoader() {
};

void ConfigLoader::applyConfiguration(ec_master_t *master, ec_domain *domain1, std::vector<SlaveConfig> *slaveConfigVector) {
	// configure all slaves and domain for each pdo entry of each slave

	std::cout << "\ngot here::" << slaveConfigVector->size() << "\n";

	ec_slave_config_t *sc;
	for (unsigned int si = 0; si < slaveConfigVector->size(); si++) {

		std::cout << "\nin loop: si = " << si << "\n";

		SlaveConfig slaveConfig = slaveConfigVector->at(si);

		// TEST ONLY
		std::vector<SyncManager> syncManagerVector = slaveConfig.syncs;

		/*
		 * code to generate sync structure for convienience api
		 *
		// here we generate the structs
		ec_sync_info_t* syncs = new ec_sync_info_t[syncManagerVector.size()];

		std::cout << "syncs size = " << syncManagerVector.size();

		for (unsigned int i=0; i<syncManagerVector.size(); i++) {
			SyncManager syncManager = syncManagerVector.at(i);
			syncs[i].dir = syncManager.dir;
			syncs[i].index = syncManager.index;
			syncs[i].n_pdos = syncManager.pdos.size();
			syncs[i].watchdog_mode = syncManager.watchdog_mode;

			syncs[i].pdos = new ec_pdo_info_t[syncs[i].n_pdos];
			ec_pdo_info_t* pdos = syncs[i].pdos;

			for (unsigned int j=0; j<syncs[i].n_pdos; j++) {

				Pdo pdo = syncManager.pdos.at(j);

				syncs[i].pdos[j].index = pdo.index;
				syncs[i].pdos[j].n_entries = pdo.n_entries;

				pdos[j].entries = new ec_pdo_entry_info_t[syncs[i].pdos[j].n_entries];
				ec_pdo_entry_info_t* pdo_entries = pdos[j].entries;

				for (unsigned int k=0; k<pdos[j].n_entries; k++) {

					PdoEntry pdoEntry = pdo.pdoEntries.at(k);

					pdo_entries[k].index = pdoEntry.index;
					pdo_entries[k].subindex = pdoEntry.subindex;
					pdo_entries[k].bit_length = pdoEntry.bitLength;
				}

			}
		}

		printSyncs(syncManagerVector.size(), syncs);
		*/

		std::cout << std::hex << slaveConfig.alias << "::" << slaveConfig.pos << "::" << slaveConfig.vendor_id << "::" << slaveConfig.product_code;


		// Create configuration for bus coupler
		sc = ecrt_master_slave_config(master, slaveConfig.alias,
				slaveConfig.pos, slaveConfig.vendor_id,
				slaveConfig.product_code);

		if (!sc) {
			std::cout << "Failed to configure slave: " << slaveConfig.alias << "::" <<
					slaveConfig.pos << "::" << slaveConfig.vendor_id << "::" <<
					slaveConfig.product_code;
			throw -1;
		}
		printf("Configuring PDOs...\n");

		/*
		 * convienience api - not currently used
		 *
		if (ecrt_slave_config_pdos(sc, syncManagerVector.size(), syncs)) {
			fprintf(stderr, "Failed to configure PDOs.\n");
			throw -1;
		}
		*/

		unsigned int absPdoIndex = 0;
		for (unsigned int i=0; i<syncManagerVector.size(); i++) {
			SyncManager syncManager = syncManagerVector.at(i);

			// create/setup a sync manager
			ecrt_slave_config_sync_manager(sc, syncManager.index, syncManager.dir, syncManager.watchdog_mode);

			// clear all pdos previously assigned (if any)
			ecrt_slave_config_pdo_assign_clear(sc, syncManager.index);

			for (unsigned int j=0; j<syncManager.pdos.size(); j++) {

				Pdo pdo = syncManager.pdos.at(j);

				ecrt_slave_config_pdo_assign_add(sc, syncManager.index, absPdoIndex);

				ecrt_slave_config_pdo_mapping_clear(sc, absPdoIndex);

				for (unsigned int k=0; k<pdo.n_entries; k++) {

					PdoEntry pdoEntry = pdo.pdoEntries.at(k);

					ecrt_slave_config_pdo_mapping_add(sc, absPdoIndex, pdoEntry.index, pdoEntry.subindex, pdoEntry.bitLength);

				}
				absPdoIndex++;
			}
		}


		// PDO Entry Domain Registration

		for (unsigned int i = 0; i < slaveConfig.pdoEntries.size(); i++) {

			PdoEntry pdoEntry = slaveConfig.pdoEntries.at(i);

			unsigned int offset = 0;
			unsigned int bitpos = 0;
			offset = ecrt_slave_config_reg_pdo_entry(sc,
					pdoEntry.index, pdoEntry.subindex, domain1,
					&bitpos);

			slaveConfigVector->at(si).pdoEntries.at(i).domainOffset = offset;
			slaveConfigVector->at(si).pdoEntries.at(i).domainBitPos = bitpos;

			std::cout << std::hex << "\n" << slaveConfigVector->at(si).pdoEntries.at(i).domainOffset << "<>" << slaveConfigVector->at(si).pdoEntries.at(i).domainBitPos;

		}

	}
	printf("\n");
}


std::vector<SlaveConfig> ConfigLoader::loadConfiguration(ec_master_t *master, std::string configFile) {

	// Decision: there is no public API for getting PDO entries, etc.  This information *could* be obtained using the source
	// from the 'tool' area, but this appears to be fairly complex and would be time consuming.  The information we want is still
	// available from the 'tool' commands, and to support new versions, it is probably cleaner to just use the command line tools
	// as is and use the tool outputs directly, rather than trying to recreate the tools in other code.

	std::vector<SlaveConfig> slaveConfigList;

	TiXmlNode* pChild;

	TiXmlDocument doc(configFile.c_str());
	doc.LoadFile();
	printf("\n%s:\n", configFile.c_str());

	TiXmlNode* root = doc.FirstChild("EtherCATInfoList");

	// loop over slaves
	int slaveIndex = -1;
	for (pChild = root->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {

		slaveIndex++;
		std::cout << "\n child value = " << pChild->Value();

		ec_slave_info_t *slave_info = new ec_slave_info_t;
		// get the slave alias
		int success = ecrt_master_get_slave (master, slaveIndex, slave_info);

		// Slave Information

		TiXmlElement* vendorElem = pChild->FirstChildElement("Vendor");
		TiXmlElement* idElem = vendorElem->FirstChildElement("Id");
		TiXmlText* idValueText = idElem->FirstChild()->ToText();

		std::cout << "\n vendor id: " << idValueText->Value();

		TiXmlElement* descriptionsElem = pChild->FirstChildElement("Descriptions");
		TiXmlElement* devicesElem = descriptionsElem->FirstChildElement("Devices");
		TiXmlElement* deviceElem = devicesElem->FirstChildElement("Device");
		TiXmlElement* typeElem = deviceElem->FirstChildElement("Type");

		TiXmlText* typeValueText = typeElem->FirstChild()->ToText();

		std::cout << "\n device type = " << typeValueText->Value();

		// product code and revision number

		std::cout << "\n product code = " << typeElem->Attribute("ProductCode");
		std::cout << "\n revision # = " << typeElem->Attribute("RevisionNo");

		// device name

		TiXmlElement* nameElem = deviceElem->FirstChildElement("Name");
		TiXmlText* nameText = nameElem->FirstChild()->ToText();

		std::cout << "\n device name = " << nameText->Value();

		// fill the structure with parsed values

		std::string *name = new std::string(nameText->Value());

		unsigned int product_code = hexCharToInt(typeElem->Attribute("ProductCode"));
		//unsigned int pos = slaveIndex;
		unsigned int pos = 0;
		unsigned int vendor_id = charToInt(idValueText->Value());
		unsigned int alias = (success == 0) ? slave_info->alias : 0;
		//unsigned int alias = 0;
		std::cout << "\nalias = " << alias;

		// loop over a slave's Sync Managers
		// the order is important in linking them to the PDOs

		// create slave config

		SlaveConfig *slaveConfig = new SlaveConfig(*name, vendor_id, product_code, alias, pos);

		std::vector<Pdo> slavePdoVector;
		std::vector<PdoEntry> slavePdoEntryVector;
		std::vector<SyncManager> slaveSyncManagerVector;

		TiXmlNode* smChild = 0;
		int i = -1;
		while ((smChild = deviceElem->IterateChildren("Sm", smChild))) {

			std::cout << "\n\nSync Manager " << ++i << "\n";

			TiXmlElement* smElem = smChild->ToElement();

			std::cout << "\n start address = " << smElem->Attribute("StartAddress");
			std::cout << "\n enable = " << smElem->Attribute("Enable");
			std::cout << "\n control byte = " << smElem->Attribute("ControlByte");
			std::cout << "\n default size = " << smElem->Attribute("DefaultSize");

			//  we must only be counting those that are for Sync manager #i
			int txCount = countPdoChildren(deviceElem, "TxPdo", i);
			int rxCount = countPdoChildren(deviceElem, "RxPdo", i);

			ec_direction_t dir = (txCount > 0) ? EC_DIR_INPUT : EC_DIR_OUTPUT;
			uint8_t index = i;
			unsigned int n_pdos = (txCount > 0) ? txCount : rxCount;
			ec_watchdog_mode_t watchdog_mode = (txCount > 0) ? EC_WD_DISABLE : EC_WD_ENABLE;


			std::vector<Pdo> pdos;
			if (txCount > 0) {
				pdos = loadPdoInfo("TxPdo", deviceElem, i, n_pdos);
			} else {
				pdos = loadPdoInfo("RxPdo", deviceElem, i, n_pdos);
			}

			SyncManager *syncManager = new SyncManager(index, dir, watchdog_mode);

			// add to running vectors of pdos and pdo entries for this slave
			for(int pdoi = 0; pdoi < (int)pdos.size(); pdoi++) {
			    Pdo pdo = pdos.at(pdoi);

			    // add to sync manager
			    syncManager->pdos.push_back(pdo);

			    // add to slave totals
			    slaveConfig->pdos.push_back(pdo);

			    std::cout << "\n&&&& " << pdo.name << ", size = " << pdo.pdoEntries.size();

			    for(int pdoei = 0; pdoei < (int)pdo.pdoEntries.size(); pdoei++) {

			    	PdoEntry pdoEntry = pdo.pdoEntries.at(pdoei);

			    	slaveConfig->pdoEntries.push_back(pdoEntry);

			    	std::cout << pdoEntry.entryName << ", ";
			    }
			}
			std::cout << "\n";

			slaveSyncManagerVector.push_back(*syncManager);
		}

		// add completed sync tree structs to the slave config
		slaveConfig->syncs = slaveSyncManagerVector;

		slaveConfigList.push_back(*slaveConfig);
	}


	return slaveConfigList;

}

std::vector<Pdo> ConfigLoader::loadPdoInfo(const char* type, TiXmlElement *deviceElem, int smIndex, int n_pdos) {

// allocate the tmt pdos array - new

	std::vector<Pdo> pdo_vector;


	std::cout << "\nnumber of pdos " << n_pdos << "\n";

	if (n_pdos == 0) {
		return pdo_vector;
	}


// loop over a slave's PDOs
	TiXmlNode* pdoChild = 0;
	int j = 0;
	for (pdoChild = deviceElem->FirstChild(type); pdoChild != 0; pdoChild = pdoChild->NextSibling()) {

		std::cout << "\n\nPdo " << j++ << "\n";

		TiXmlElement* pdoElem = pdoChild->ToElement();

		int syncMgr = charToInt(pdoElem->Attribute("Sm"));
		if (syncMgr != smIndex) continue;

		std::cout << "\n Sync Manager = " << pdoElem->Attribute("Sm");
		std::cout << "\n fixed = " << pdoElem->Attribute("Fixed");
		std::cout << "\n mandatory = " << pdoElem->Attribute("Mandatory");



		TiXmlText* indexText = pdoElem->FirstChild("Index")->FirstChild()->ToText();

		std::cout << "\n index = " << indexText->Value();

		unsigned int index = hexCharToInt(indexText->Value());

		std::cout << "    >>>>>> 0x" << std::hex << index;

		std::string *name = new std::string("Not Specified");

		TiXmlText* nameText = pdoElem->FirstChild("Name")->FirstChild()->ToText();
		std::cout << "\n name = " << nameText->Value();
		name = new std::string(nameText->Value());


		int pdoEntryCount = countChildren(pdoElem, "Entry");
		int n_entries = pdoEntryCount;


		// save this pdo
		Pdo *pdo = new Pdo(*name, index, n_entries);
		pdo_vector.push_back(*pdo);



		// loop over all PDO entries


		TiXmlNode* pdoEntryChild = 0;
		int k = 0;
		for (pdoEntryChild = pdoElem->FirstChild("Entry"); pdoEntryChild != 0; pdoEntryChild = pdoEntryChild->NextSibling()) {


			std::cout << "\n-----------------\n";
			std::cout << "Pdo Entry " << k++ << "\n";


			TiXmlText* indexText = pdoEntryChild->FirstChild("Index")->FirstChild()->ToText();
			std::cout << "\n index = " << indexText->Value();

			TiXmlText* bitLenText = pdoEntryChild->FirstChild("BitLen")->FirstChild()->ToText();
			std::cout << "\n bit length = " << bitLenText->Value();

			int bit_length = charToInt(bitLenText->Value());
			unsigned int index = hexCharToInt(indexText->Value());

			int subindex = 0;

			if (pdoEntryChild->FirstChild("SubIndex")) {

				TiXmlText* subIndexText = pdoEntryChild->FirstChild("SubIndex")->FirstChild()->ToText();
				std::cout << "\n sub-index = " << subIndexText->Value();
				subindex = charToInt(subIndexText->Value());

			} else {
				std::cout << "\n sub-index = " << "0x00";
			}

			std::string *entry_name = new std::string("Not Specified");

			if (pdoEntryChild->FirstChild("Name")) {

				TiXmlText* nameText = pdoEntryChild->FirstChild("Name")->FirstChild()->ToText();
				std::cout << "\n name = " << nameText->Value();
				entry_name = new std::string(nameText->Value());
			} else {
				std::cout << "\n name = " << "Not Specified " << k;
			}


			if (pdoEntryChild->FirstChild("DataType")) {
				TiXmlText* dataTypeText = pdoEntryChild->FirstChild("DataType")->FirstChild()->ToText();
				std::cout << "\n data type = " << dataTypeText->Value();
			} else {
				std::cout << "\n data type = " << "None";
			}

			// save this as pdoEntry
			PdoEntry *pdoEntry = new PdoEntry(*entry_name, pdo->name, index, subindex, bit_length);
			pdo_vector.back().pdoEntries.push_back(*pdoEntry);

			std::cout << "\n---------" << pdo_vector.size() << "--------\n";

		}
		std::cout << "all PDO entries done";

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

	std::string *sindex = new std::string(input);

	sindex->replace(0, 1, "0");

	std::stringstream strValue;
	strValue << *sindex;

	strValue >> std::hex >> intValue;

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

	std::cout << "in printSyncs nsyncs = " << n_syncs;

	for (int i=0; i<n_syncs; i++) {

		printf("\nnsync = %hx :: %hx :: %hx :: %hx", sync[i].dir, sync[i].index, sync[i].n_pdos, sync[i].watchdog_mode);


		for (unsigned int j=0; j<sync[i].n_pdos; j++) {

			ec_pdo_info_t *pdos = sync[i].pdos;
			printf("\n\tpdo = %hx :: %hx", pdos[j].index, pdos[j].n_entries);

			for (unsigned int k=0; k<pdos[j].n_entries; k++) {
				ec_pdo_entry_info_t *pdo_entry = pdos[j].entries;
				printf("\n\t\tentry = %hx :: %hx :: %hx", pdo_entry[k].index, pdo_entry[k].subindex, pdo_entry[k].bit_length);
			}
		}
	}

	printf("\n");
}
