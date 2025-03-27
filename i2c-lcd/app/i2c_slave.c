#include "app/heartbeat.h"
#include "intrinsics.h"
#include "msp430fr2310.h"
#include "i2c_slave.h"
#include "LCD_driver.h"
#include <msp430.h>

int start = 0;
int i, j;
char Data_In[] = "eee";
char Datum_In = '0';
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

// -- START I2C ISR --
#pragma vector=EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void){
    switch(UCB0IV){
        case 0x16:              // ID 16: RXIFG0 asserts after from slave
            Datum_In = UCB0RXBUF;    //receive data and store in Data_In
            lcd_send_data(Datum_In);
            lcd_clear_bottom();
            lcd_clear_top();
            switch(Datum_In){
                case '0':
                    lcd_send_string("Static");
                    break;
                case '1':
                    lcd_send_string("Toggle");
                    break;
                case '2':
                    lcd_send_string("Up Count");
                    break;
                case '3':
                    lcd_send_string("In Out");
                    break;
                case '4':
                    lcd_send_string("Down Count");
                    break;
                case '5':
                    lcd_send_string("Rotate Left");
                    break;
                case '6':
                    lcd_send_string("Rotate Right");
                    break;
                case '7':
                    lcd_send_string("Fill");
                    break;
                case '9':
                    lcd_toggle_blink();
                    break;
                case 'C':
                    lcd_toggle_cursor();
                    break;
                case 'D':
                    break;
            }
            if(Datum_In!='D') lcd_set_key_char(Datum_In);
            heartbeat_run();
        case 0x18:              // ID 18: TXIFG0 asserts when register val can be sent
            UCB0TXBUF = 0x03;   // register address
            break;
        default:
            break;
    }
}
// -- END I2C ISR --