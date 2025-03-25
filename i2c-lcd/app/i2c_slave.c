#include "app/heartbeat.h"
#include "intrinsics.h"
#include "msp430fr2310.h"
#include "i2c_slave.h"
#include <msp430.h>

int start = 0;
int i, j;
char Data_In[] = "eee";
int Data_Cnt = 0;
int Out_Cnt = 0;
char Packet[] = {0x03, 0x0, 0x10, 0x13, 0x01, 0x03, 0x05, 0x24};

void i2c_slave_init(){
    __disable_interrupt();
    UCB0CTLW0 = UCSWRST;                    // Software reset enabled
    UCB0CTLW0 |= UCMODE_3 | UCSYNC;         // I2C mode, sync mode
    UCB0CTLW0 &= ~UCMST;                    // Put into slave mode
    UCB0I2COA0 = 0x40 | UCOAEN;             // Slave address = 0x1101000b

    UCB0CTLW1 |= UCASTP_2;                  // Enable AutoStop
    UCB0TBCNT = sizeof(Packet);             // Auto Stop when UCB0TBCNT reached

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

void i2c_slave_receive(){
    // -- Send starting register --
    // UCB0I2CSA = 0x0068;         // Slave address = 0x1101000b
    // UCB0TBCNT = sizeof(Packet);
    // UCB0CTLW0 |= UCTR;          // Tx mode
    // UCB0CTLW0 |= UCTXSTT;       // Start condition
    // while ((UCB0IFG & UCSTPIFG) == 0) 
    //     __delay_cycles(100);    // wait for STOP
    // UCB0IFG &= ~UCSTPIFG;       // Clear STOP flag
}

// -- START I2C ISR --
#pragma vector=EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void){
    switch(UCB0IV){
        case 0x16:              // ID 16: RXIFG0 asserts after from slave
            Data_In[Out_Cnt] = UCB0RXBUF;    //receive data and store in Data_In
            heartbeat_run();
            Out_Cnt++;
            break;
        case 0x18:              // ID 18: TXIFG0 asserts when register val can be sent
            UCB0TXBUF = 0x03;   // register address
            break;
        default:
            break;
    }
}
// -- END I2C ISR --