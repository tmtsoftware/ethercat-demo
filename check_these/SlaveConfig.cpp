#include <string>

#include "SlaveConfig.h"

SlaveConfig::SlaveConfig(std::string name, unsigned int vendor_id, unsigned int product_code, unsigned int alias, unsigned int pos) {
	this->name = name;
	this->vendor_id = vendor_id;
	this->product_code = product_code;
	this->alias = alias;
	this->pos = pos;
	this->domain_offset = 0;
	this->domain_bit_pos = 0;
};
