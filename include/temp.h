/*
 * temp.h
 *
 *  Created on: Nov 3, 2014
 *      Author: smichaels
 */

#ifndef INCLUDE_TEMP_H_
#define INCLUDE_TEMP_H_

 #define FREQUENCY 100

// structure to hold all slave info
typedef struct {
    const char *name;
    unsigned int vendor_id;
    unsigned int product_code;
    unsigned int alias;
    unsigned int pos;
    unsigned int *domain_offset;
    unsigned int *domain_bit_pos;
    ec_sync_info_t *syncs;
    ec_pdo_info_t *pdos;
    ec_pdo_entry_info_t *pdo_entries;
    char *pdo_entry_names[50];
    int pdo_entry_length;  // when I remember C better I wont need this
} tmt_ec_slave_t;







#endif /* INCLUDE_TEMP_H_ */
