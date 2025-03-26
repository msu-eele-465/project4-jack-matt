#ifndef I2CMASTER
#define I2CMASTER

void i2c_master_init();

void i2c_master_transmit(int address, char packet[]);

#endif // HEARTBEAT_H#include "intrinsics.h"