/*
 *  magdriver - 
 *  Copyright (c) 2013 - Stefano Bodini
 *
 *  This code is designed to drive three coils with a 1 sec pulse each using an LaunchPad board.
 *  The board is a LaunchPad board, using a MSP430F2012 and the code is compiled using MSPGCC 4.4.3
 *  using the -mmcu=msp430x2012  parameter
 */
//#include <io.h>
#include<msp430.h>
#include <signal.h>

/*
 *  Defines for I/O
 *   P1.0  -> Coil1  (OUTPUT - red LED)
 *   P1.1  -> Coil2  (OUTPUT)
 *   P1.2  -> Coil3  (OUTPUT)
 *   P1.3  -> not used - input
 *   P1.4  -> not used - input
 *   P1.5  -> not used - input
 *   P1.6  -> not used - input
 *   P1.7  -> not used - input
 */
#define     COIL_PORT             P1OUT
#define     MASK_CLEAN            0xF8   /* Filter out only outputs */
#define     MASK1                 0x01   /* Coil 1 ON, Coil 2 OFF, Coil 3 OFF */
#define     MASK2                 0x02   /* Coil 1 OFF, Coil 2 ON, Coil 3 OFF */
#define     MASK3                 0x04   /* Coil 1 OFF, Coil 2 OFF, Coil 3 ON */
/*
 *  Global variables
 */

unsigned char Coil;      	/* Coil index */

int main(void)
{
   WDTCTL = WDTPW + WDTHOLD;	/* Stop WDT */
 
   BCSCTL1 |= DIVA_3;		/* ACLK/8 */
   BCSCTL3 |= XCAP_3;		/* 12.5pF cap- setting for 32768Hz crystal */
 
   P1DIR |= BIT0 + BIT1 + BIT2;	/* set P1.0, P1.1 and P1.2 as output */
   P1OUT &= MASK_CLEAN;         /* Force low coils */

   Coil = MASK1; 		/* Start from the first coil */
   
   CCTL0 = CCIE;		/*  CCR0 interrupt enabled */
   CCR0 = 512;			/* 512 -> 1 sec, 30720 -> 1 min */
   TACTL = TASSEL_1 + ID_3 + MC_1;	/* ACLK, /8, upmode */
 
   _BIS_SR(LPM3_bits + GIE);	/* Enter LPM3 w/ interrupt */
   /* Main goes to die - only interrupt is working */
	
	for(;;);
}

/*
 *  Timer interrupt - every second
 */ 
__attribute__((__interrupt__(TIMERA0_VECTOR)))
void Timer_A (void)
{
   unsigned char shadow = COIL_PORT & MASK_CLEAN;  /* Read current content and force to zero last 3 bits */
   shadow |= Coil;                                 /* Load current mask */
   COIL_PORT = shadow;                             /* Put out signal */

   switch(Coil)                                    /* Update mask for next interrupt */
   {
      case MASK1:
         Coil = MASK2;
         break; 

      case MASK2:
         Coil = MASK3;
         break; 

      case MASK3:
      default:
         Coil = MASK1;
         break; 
   }
}


