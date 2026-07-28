#include "p33CK256MP506.h"

#pragma config   FNOSC     = FRC
#pragma config   IESO      = ON
#pragma config   POSCMD    = NONE
#pragma config   FCKSM     = CSECMD
#pragma config   FWDTEN    = ON_SW
#pragma config   ICS       = PGD1
#pragma config   JTAGEN    = OFF

typedef unsigned short       UINT16;
typedef unsigned long        UINT32;

static  UINT16               UBOOTECAN1MSGBUF[256] __attribute__((aligned(4)));
static  UINT16               UBOOTECAN1MSGSFSERVICE;
static  UINT16               UBOOTECAN1MSGSFNUMBCNT;
static  UINT16               UBOOTECAN1MSGSFNUMBODD;
static  UINT16               UBOOTECAN1MSGSFADDRRES;
static  UINT16               UBOOTECAN1MSGSFSECTCNT;
static  UINT16               UBOOTECAN1MSGSFWAITCNT;
static  UINT32               UBOOTECAN1MSGCRC32DATA;
static  UINT32               UBOOTECAN1MSGCRC32BCNT;
static  UINT32               UBOOTECAN1MSGCRC32RBUF;
static  UINT32               UBOOTECAN1MSGRDKEYSEED;
static  UINT32               UBOOTECAN1MSGRDKEYRBUF;

extern  void                 App(void);

int main(void)
{
    CLKDIVbits.PLLPRE      = 1;
    PLLFBDbits.PLLFBDIV    = 125;
    PLLDIVbits.POST1DIV    = 5;
    PLLDIVbits.POST2DIV    = 1;
    __builtin_write_OSCCONH(0x01);
    __builtin_write_OSCCONL(OSCCON | 0x01);
    while(OSCCONbits.OSWEN != 0);
    while(OSCCONbits.LOCK != 1);
    ACLKCON1bits.FRCSEL    = 1;
    ACLKCON1bits.APLLPRE   = 1;
    APLLFBD1bits.APLLFBDIV = 100;
    APLLDIV1bits.APOST1DIV = 2;
    APLLDIV1bits.APOST2DIV = 1;
    ACLKCON1bits.APLLEN    = 1;
    CANCLKCON              = 0x8204;

    RPINR26bits.CAN1RXR    = 52;
    RPOR10bits.RP53R       = 21;
    C1CONLbits.CON         = 1;
    C1CONHbits.REQOP       = 4;
    while(C1CONHbits.OPMOD != 4);
    C1FIFOBAL              = (UINT16)UBOOTECAN1MSGBUF;
    C1NBTCFGH              = 0x003E;
    C1NBTCFGL              = 0x0F0F;
  //C1NBTCFGH              = 0x007E;
  //C1NBTCFGL              = 0x1F1F;
  //C1NBTCFGH              = 0x00FE;
  //C1NBTCFGL              = 0x3F3F;
    C1CONLbits.BRSDIS      = 1;
    C1CONHbits.TXQEN       = 1;
    C1TXQCONHbits.FSIZE    = 2;
    C1TXQCONHbits.PLSIZE   = 0;
    C1FIFOCON1Hbits.FSIZE  = 2;
    C1FIFOCON1Hbits.PLSIZE = 0;
    C1FIFOCON2Hbits.FSIZE  = 2;
    C1FIFOCON2Hbits.PLSIZE = 0;
    C1FLTCON0Lbits.F0BP    = 1;
    C1FLTOBJ0H             = 0x5AD7;
    C1FLTOBJ0L             = 0xCF3F;
    C1MASK0H               = 0x5FFF;
    C1MASK0L               = 0xFFFF;
    C1FLTCON0Lbits.FLTEN0  = 1;
    C1FLTCON0Lbits.F1BP    = 2;
    C1FLTOBJ1H             = 0x56B7;
    C1FLTOBJ1L             = 0xCF39;
    C1MASK1H               = 0x5FFF;
    C1MASK1L               = 0xFFFF;
    C1FLTCON0Lbits.FLTEN1  = 1;
    C1CONHbits.REQOP       = 6;
    while(C1CONHbits.OPMOD != 6);

    PR1                    = 50000;
    IFS0bits.T1IF          = 0;
    T1CONbits.TON          = 1;

    UBOOTECAN1MSGBUF[0]    = 0;
    UBOOTECAN1MSGSFSERVICE = 0;
    UBOOTECAN1MSGSFNUMBCNT = 0;
    UBOOTECAN1MSGSFNUMBODD = 0;
    UBOOTECAN1MSGSFADDRRES = 0;
    UBOOTECAN1MSGSFSECTCNT = 0;
    UBOOTECAN1MSGSFWAITCNT = 0;
    UBOOTECAN1MSGCRC32DATA = 0xFFFFFFFF;
    UBOOTECAN1MSGCRC32BCNT = 0;
    UBOOTECAN1MSGCRC32RBUF = 0;
    UBOOTECAN1MSGRDKEYSEED = 0;
    UBOOTECAN1MSGRDKEYRBUF = 0;
    
    for(;;)
    {
        App();
    }
}