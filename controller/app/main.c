#include "heartbeat.h"
#include "keypad.h"
#include "RGB.h"
#include "i2c_master.h"
#include <msp430fr2355.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer

    // Disable low-power mode / GPIO high-impedance
    PM5CTL0 &= ~LOCKLPM5;
    i2c_master_init();
    heartbeat_init();
    keypad_init();
    i2c_master_transmit("hi");

    while(1) {
        // Main loop
        keypad_scan();
    }
}