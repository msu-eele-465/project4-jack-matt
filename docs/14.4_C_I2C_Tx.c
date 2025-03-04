#include <msp430.h> 


/**
 * Jack Tjaden Lab 14.4 Fall 2023
 */

int data_cnt = 0;
char packet[] = {0x03, 0b110010, 0b11101, 0b1100, 0b11, 0b101, 0b10001, 0b10111};

            // register, seconds, minute, hours, day, weekday, month, year
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	UCB0CTLW0 |= UCSWRST; // Put eUSCI into  SW reset

	UCB0CTLW0 |= UCSSEL__SMCLK; // configure eUSCI_A0
	UCB0BRW = 10;


	UCB0CTLW0 |= UCMODE_3;
	UCB0CTLW0 |= UCMST;
	UCB0CTLW0 |= UCTR;
	UCB0I2CSA = 0x0068;

	UCB0CTLW1 |= UCASTP_2;
	UCB0TBCNT = sizeof(packet);

	P1SEL1 &= ~BIT3; // p1.3 = SCL
	P1SEL0 |= BIT3;

	P1SEL1 &= ~BIT2; // p1.2 = SDA
	P1SEL0 |= BIT2;

	PM5CTL0 &= ~LOCKLPM5;

	UCB0CTLW0 &= ~UCSWRST;

	UCB0IE |= UCTXIE0;

	__enable_interrupt();

	int i;
	while(1){
	    UCB0CTLW0 |= UCTXSTT;
	    for(i = 0; i < 100; i = i +1){}
	}

	return 0;
}

#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void){

    if(data_cnt == (sizeof(packet) - 1)){
        UCB0TXBUF = packet[data_cnt];
        data_cnt = 0;
    }
    else{
        UCB0TXBUF = packet[data_cnt];
        data_cnt++;
    }


}

