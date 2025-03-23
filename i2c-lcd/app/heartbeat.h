#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <stdint.h>
#include <stddef.h>

// a function to initialize the P6.6 and TIMER1_B0_VECTOR isr
void heartbeat_init(void);

// a function to alternate the P6.6 LED
void heartbeat_run(void);

#endif // HEARTBEAT_H