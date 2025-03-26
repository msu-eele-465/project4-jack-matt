#include <msp430fr2310.h>
#include <stdbool.h>

#include "heartbeat.h"
#include "LEDarray.h"

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    heartbeat_init();
    ledarray_init();

    ledarray_select_pattern(PATTERN_7_FILL);

    // Disable low-power mode / GPIO high-impedance
    PM5CTL0 &= ~LOCKLPM5;

    while (true)
    {

    }
}
