/*
 * temp.h
 *
 *  Created on: Nov 3, 2014
 *      Author: smichaels
 */

#ifndef INCLUDE_TEMP_H_
#define INCLUDE_TEMP_H_

 #define FREQUENCY 100

using namespace std;

typedef struct {
	string* entry_name;
	string* full_name;
	uint16_t index;
	uint8_t subindex;
	uint8_t bit_length;
	int domain_offset;
	int domain_bit_pos;
} tmt_ec_pdo_entry;

typedef struct {
	string* name;
	uint16_t index;
	unsigned int n_entries;
	tmt_ec_pdo_entry *pdo_entries;
} tmt_ec_pdo;

typedef struct {
	uint8_t index;
	ec_direction_t dir;
	unsigned int n_pdos;
	tmt_ec_pdo *pdos;
	ec_watchdog_mode_t watchdog_mode;
} tmt_ec_sync;

// structure to hold all slave info
// We need a full set of ec_sync_info_t structs, decending through ec_pdo_info_t's and ec_pdo_entry_info_t's for the PDO Configuration step
// We can also use an array of tmt_ec_pdo_entry structs for Domain Registration - these will include name and offset/bit_pos
// The name of a PDO Entry in the tmt_ec_pdo_entry struct will be the composite of the pdo name and the pdo entry name
typedef struct {
    string *name;
    unsigned int vendor_id;
    unsigned int product_code;
    unsigned int alias;
    unsigned int pos;
    unsigned int *domain_offset;
    unsigned int *domain_bit_pos;
    ec_sync_info_t *syncs;
    ec_pdo_info_t *pdos;				// not needed in xml construction, but used in cstruct
    ec_pdo_entry_info_t *pdo_entries; 	// not needed in xml construction, but used in cstruct
    char *pdo_entry_names[50];			// deprecated - remove this
    int pdo_entry_length;

    tmt_ec_sync *tmt_syncs;  // might not need this?
    tmt_ec_pdo *tmt_pdos;
    tmt_ec_pdo_entry *tmt_pdo_entries;
} tmt_ec_slave_t;


#define ENABLE_NEW_CONFIG_CODE 1
#define ENABLE_NEW_RUN_CODE 1





#endif /* INCLUDE_TEMP_H_ */
