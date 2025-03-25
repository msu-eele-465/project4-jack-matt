#include "heartbeat.h"
#include "keypad.h"
#include "RGB.h"
#include "i2c_master.h"
#include <msp430fr2355.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer
    i2c_master_init();

    // Disable low-power mode / GPIO high-impedance
    PM5CTL0 &= ~LOCKLPM5;
    // heartbeat_init();
    // keypad_init();
    i2c_master_transmit(0x68, "hi");

    while(1) {
        // Main loop
        //keypad_scan();
        i2c_master_transmit(0x40, "hi");
    }
}