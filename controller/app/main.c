#include "heartbeat.h"
#include "keypad.h"
#include "RGB.h"
#include <msp430fr2355.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer

    // Disable low-power mode / GPIO high-impedance
    PM5CTL0 &= ~LOCKLPM5;

    heartbeat_init();
    keypad_init();

    while(1) {
        // Main loop
        keypad_scan();
    }
}