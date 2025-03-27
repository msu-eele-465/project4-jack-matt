#include "intrinsics.h"
#include "i2c_master.h"
#include <msp430.h>

int start = 0;
int i, j;
char Data_In[] = "eee";
int Data_Cnt = 0;
int Out_Cnt = 0;
char Packet[] = {0x03, 0x0, 0x10, 0x13, 0x01, 0x03, 0x05, 0x24};

void i2c_master_init(){
    UCB0CTLW0 |= UCSWRST;
    // configure eUSCI_B0 -----------------------------------------------
    UCB0CTLW0 |= UCSSEL_3;      // Choose BRCLK=SMCLK=1MHz
    UCB0BRW = 10;               // Divide BRCLK by 10 for SCL=100kHz

    UCB0CTLW0 |= UCMODE_3;      // Put into I2C mode
    UCB0CTLW0 |= UCMST;         // Put into master mode
    UCB0CTLW0 |= UCTR;          // Tx mode
    UCB0I2CSA = 0x0068;         // Slave address = 0x1101000b

    UCB0CTLW1 |= UCASTP_2;      // Enabling auto stop

    //configure ports
    P1SEL1 &= ~BIT3;            // we want P1.3 = SCL
    P1SEL0 |= BIT3;

    P1SEL1 &= ~BIT2;            // we want P1.2 = SDA
    P1SEL0 |= BIT2;

    PM5CTL0 &= ~LOCKLPM5;
    UCB0CTLW0 &= ~UCSWRST;

    UCB0IE |= UCTXIE0;          // Enable Tx interrupt
    UCB0IE |= UCRXIE0;          // enable Rx interrupt
    __enable_interrupt();       // Enable Maskable IRQs
}

void i2c_master_transmit(int address, char packet[]){
    // -- Send starting register --
    UCB0I2CSA = address;         // Slave address
    Packet[0] = packet[0];
    UCB0TBCNT = 1;
    UCB0CTLW0 |= UCTR;          // Tx mode
    UCB0CTLW0 |= UCTXSTT;       // Start condition
    while ((UCB0IFG & UCSTPIFG) == 0) 
        __delay_cycles(100);    // wait for STOP
    UCB0IFG &= ~UCSTPIFG;       // Clear STOP flag
}

// -- START I2C ISR --
#pragma vector=EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void){

    if(Data_Cnt<(1)){                   // if first time in interrupt
        __delay_cycles(30);
        UCB0TXBUF = Packet[Data_Cnt];  // transmit each value in packet
        Data_Cnt++;
        if(Data_Cnt==1) Data_Cnt=0;
    }else{
        switch(UCB0IV){
            case 0x16:              // ID 16: RXIFG0 asserts after from slave
                Data_In[Out_Cnt] = UCB0RXBUF;    //receive data and store in Data_In
                Out_Cnt++;
                break;
            case 0x18:              // ID 18: TXIFG0 asserts when register val can be sent
                UCB0TXBUF = 0x03;   // register address
                break;
            default:
                break;
        }
    }
}
// -- END I2C ISR --