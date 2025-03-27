#include "heartbeat.h"
#include "msp430fr2310.h"
#include "i2c_slave.h"
#include "LEDarray.h"

char Datum_In = '0';

void i2c_slave_init(){
    __disable_interrupt();
    UCB0CTLW0 = UCSWRST;                    // Software reset enabled
    UCB0CTLW0 |= UCMODE_3 | UCSYNC;         // I2C mode, sync mode
    UCB0CTLW0 &= ~UCMST;                    // Put into slave mode
    UCB0I2COA0 = 0x42 | UCOAEN;             // Slave address = 0x1101000b

    UCB0CTLW1 |= UCASTP_2;                  // Enable AutoStop

    //configure ports
    P1SEL1 &= ~BIT3;            // we want P1.3 = SCL
    P1SEL0 |= BIT3;

    P1SEL1 &= ~BIT2;            // we want P1.2 = SDA
    P1SEL0 |= BIT2;

    UCB0CTLW0 &= ~UCSWRST;                  // clear reset register

    UCB0IE |= UCTXIE0;          // Enable Tx interrupt
    UCB0IE |= UCRXIE0;          // enable Rx interrupt
    __enable_interrupt();       // Enable Maskable IRQs

    UCB0CTLW0 &= ~UCTR;         // Rx mode
}

// -- START I2C ISR --
#pragma vector=EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void){
    switch(UCB0IV){
        case 0x16:              // ID 16: RXIFG0 asserts after from slave
            Datum_In = UCB0RXBUF;    //receive data and store in Data_In
            ledarray_set_pattern(Datum_In);
            switch(Datum_In){
                case '0':
                    ledarray_select_pattern(PATTERN_0_STATIC);
                    break;
                case '1':
                    ledarray_select_pattern(PATTERN_1_TOGGLE);
                    break;
                case '2':
                    ledarray_select_pattern(PATTERN_2_UP_COUNT);
                    break;
                case '3':
                    ledarray_select_pattern(PATTERN_3_IN_OUT);
                    break;
                case '4':
                    ledarray_select_pattern(PATTERN_4_DOWN_COUNT);
                    break;
                case '5':
                    ledarray_select_pattern(PATTERN_5_RLA);
                    break;
                case '6':
                    ledarray_select_pattern(PATTERN_6_RRC);
                    break;
                // *Note* ran out of memory
                // case '7':
                //     ledarray_select_pattern(PATTERN_7_FILL);
                //     break;
                case 'D':
                    ledarray_select_pattern(PATTERN_NONE);
                    break;
                case 'A':
                    ledarray_decrease_period();
                    break;
                case 'B':
                    ledarray_increase_period();
                    break;
            }
            heartbeat_run();
        case 0x18:              // ID 18: TXIFG0 asserts when register val can be sent
            UCB0TXBUF = 0x03;   // register address
            break;
        default:
            break;
    }
}
// -- END I2C ISR --