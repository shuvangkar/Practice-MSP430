/*
 * hardwareSerial.c
 *
 *  Created on: Aug 25, 2020
 *      Author: Shuvangkar
 */

#include "hardwareSerial.h"
#include "my_string.h"


//char *tx_ptr = NULL;

#define BAUD 9600

#if BAUD == 9600
    #define UCA0BR0_VAL     104
    #define UCA0BR1_VAL     0
    #define UCA0MCTL_VAL    UCBRS_1
//    #define UCA0BR0_VAL     0x41
//    #define UCA0BR1_VAL     0x3
//    #define UCA0MCTL_VAL    UCBRS_2
#elif BAUD == 19200
    #define UCA0BR0_VAL     52
    #define UCA0BR1_VAL     0
    #define UCA0MCTL_VAL    UCBRS_0
#elif BAUD == 38400
    #define UCA0BR0_VAL     26
    #define UCA0BR1_VAL     0
    #define UCA0MCTL_VAL    UCBRS_0
#elif BAUD == 56000
    #define UCA0BR0_VAL     17
    #define UCA0BR1_VAL     0
    #define UCA0MCTL_VAL    UCBRS_7
#elif BAUD == 115200
    #define UCA0BR0_VAL     8
    #define UCA0BR1_VAL     0
    #define UCA0MCTL_VAL    UCBRS_6
#else
    #error BAUD RATE NOT SUPPORTED
#endif


void serial_begin(void)
{
    UCA0CTL1 |= UCSWRST;        //USCI logic held in reset state means disabled
    P1SEL |= BIT1+BIT2;         //PxSEL & PxSEL2 both registers select the pin attribute
    P1SEL2 |= BIT1+BIT2;        //P1.1 = RXD | P1.2 = TXD

    UCA0CTL1 |= UCSSEL_2;       //SMCLK clock source

    UCA0BR0 = UCA0BR0_VAL;      // 104 defined in datasheet for 9600 baudrate while SMCLK is selected
    UCA0BR1 = UCA0BR1_VAL;      //ignore
    UCA0MCTL = UCA0MCTL_VAL;    //modulation stage comes from baud rate table.


    UCA0CTL0 = 0x00;            //ignore
    UCA0CTL1 &= ~UCSWRST;       // Initialize USCI state machine

//    IE2 |= UCA0TXIE;                  // Enable the Transmit interrupt
//    _BIS_SR(GIE);                     // Enable the global interrupt
}

void serial_print_char(char c)
{
    UCA0TXBUF = c;
    while(UCA0STAT & UCBUSY);
}

void serial_print(char *str)
{
    while(*str)
    {
        UCA0TXBUF = *str;
        while(UCA0STAT & UCBUSY); //UCBUSY Flag = 1(Transmit/Receive) | 0(Inactive)
        str++;
    }
}


void serial_print_ulong(uint32_t n)
{
    char buf[13];
    char *ptr = &buf[sizeof(buf) - 1];
    *ptr = '\0';

    uint32_t  temp;
    char c;
    do
    {
        temp = n;
        n = n/10;
        c = temp - n*10;
        *--ptr = c + '0';
    }while(n);
    serial_print(ptr);
}

void serial_print_long(int32_t n)
{
    if(n<0)
    {
        serial_print_char('-');
        n = -n;
    }
    serial_print_ulong(n);
}
void serial_print_uint(uint16_t n)
{
    serial_print_ulong(n);
}

void serial_print_int(int16_t n)
{
    serial_print_long(n);
}



//void serial_print_int(int var)
//{
//    char buf[10];
//    serial_print(int_to_str(var,buf));
//}
//
//void serial_print_int2(int n)
//{
//    char buf[10];
//    char *ptr = &buf[sizeof(buf)-1];
//    *ptr = '\0';
//    bool neg = false;
//    if(n < 0)
//    {
//        n = -n;
//        neg = true;
//    }
//    do
//    {
//        uint16_t m = n;
//        n = n/10;
//        char c = m-10*n;
//        *--ptr = c+'0';
//    }while(n);
//
//    if(neg)
//    {
//        *--ptr = '-';
//    }
//    serial_print(ptr);
//}

void set_mcu_clock(void)
{
    //Set MCLK = SMCLK = 1MHz
    BCSCTL1 = CALBC1_1MHZ; // Set DCO Clock to 1MHz
    DCOCTL = CALDCO_1MHZ;
}

//#pragma vector = USCIAB0TX_VECTOR
//__interrupt void TransmitInterrupt(void)
//{
//    UCA0TXBUF = *tx_ptr;
//    tx_ptr ++;
//}
