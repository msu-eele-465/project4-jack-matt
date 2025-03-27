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
    heartbeat_init();
    keypad_init();
    // i2c_master_transmit(0x68, "hi");
    char previous = " ";
    char keypressed = " ";
    int already_unlocked = 0;
    while(1) {
        // Main loop
        if (keypressed != previous && keypressed != 0 && already_unlocked && keypad_is_unlocked()){
            switch(keypressed){
                case '1':
                    i2c_master_transmit(0x40, "1");
                    i2c_master_transmit(0x42, "1");
                    break;
                case '2':
                    i2c_master_transmit(0x40, "2");
                    i2c_master_transmit(0x42, "2");
                    break;
                case '3':
                    i2c_master_transmit(0x40, "3");
                    i2c_master_transmit(0x42, "3");
                    break;
                case 'A':
                    i2c_master_transmit(0x40, "A");
                    i2c_master_transmit(0x42, "A");
                    break;
                case '4':
                    i2c_master_transmit(0x40, "4");
                    i2c_master_transmit(0x42, "4");
                    break;
                case '5':
                    i2c_master_transmit(0x40, "5");
                    i2c_master_transmit(0x42, "5");
                    break;
                case '6':
                    i2c_master_transmit(0x40, "6");
                    i2c_master_transmit(0x42, "6");
                    break;
                case 'B':
                    i2c_master_transmit(0x40, "B");
                    i2c_master_transmit(0x42, "B");
                    break;
                case '7':
                    i2c_master_transmit(0x40, "7");
                    i2c_master_transmit(0x42, "7");
                    break;
                case '8':
                    i2c_master_transmit(0x40, "8");
                    i2c_master_transmit(0x42, "8");
                    break;
                case '9':
                    i2c_master_transmit(0x40, "9");
                    i2c_master_transmit(0x42, "9");
                    break;
                case 'C':
                    i2c_master_transmit(0x40, "C");
                    i2c_master_transmit(0x42, "C");
                    break;
                case '*':
                    i2c_master_transmit(0x40, "*");
                    i2c_master_transmit(0x42, "*");
                    break;
                case '0':
                    i2c_master_transmit(0x40, "0");
                    i2c_master_transmit(0x42, "0");
                    break;
                case '#':
                    i2c_master_transmit(0x40, "#");
                    i2c_master_transmit(0x42, "#");
                    break;
                case 'D':
                    i2c_master_transmit(0x40, "D");
                    i2c_master_transmit(0x42, "D");
                    break;
            }
        } else if (keypad_is_unlocked()){
            already_unlocked = 1;
        } else if (!keypad_is_unlocked() && keypressed=='D'){
            i2c_master_transmit(0x40, "D");
            i2c_master_transmit(0x42, "D");
        }else {
            already_unlocked = 0;
        }
        keypressed = keypad_scan();
    }
}