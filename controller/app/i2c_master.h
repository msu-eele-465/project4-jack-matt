#ifndef I2CMASTER
#define I2CMASTER

void i2c_master_init();

void i2c_master_transmit(char* packet);

#endif // HEARTBEAT_H#include "intrinsics.h"