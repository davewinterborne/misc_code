/*
 * init_SPI.c
 *
 *  Created on: 26 Apr 2022
 *      Author: nnc57
 *
 *  In this case the SPI is used for serial communication between the resolver and the DSP.
 *  It is possible to read both angle and speed from the two configurations, as well as any fault happening onto the resolver and RDC itself
 *  For the implementation of angle(position) and speed reading refer to sources -> operational functions -> resolver_read.c
 *
 *  Resolver:
 *  Set up SPI-C and GPIO for use with AD2S1210 (RDC) on GCB
 *  WARNING!! this code must NOT be executed on LVB which is NOT pin compatible.
 *  IMPORTANT!! - The SPICCR is set again at the end of this function in order to relinquish
 *  from SPI reset. Ensure that other bits are consistent with the earlier SPICCR settings
 *
 *  AD2S1210 configuration modes
 *  A0=0 A1=0 Normal mode - position output
 *  A0=0 A1=1 Normal mode - velocity output
 *  A0=1 A1=0 Reserved
 *  A0=1 A1=1 Configuration mode
 *
 *  AD2S1210 Resolution (control register bit settings must correspond to RES0, RES1 pins)
 *
 *  RES0=0 RES1=0  10-bit
 *  RES0=0 RES1=1  12-bit
 *  RES0=1 RES1=0  14-bit
 *  RES0=1 RES1=1  16-bit
 *
 *  InitRDC() is for now the only function for SPI peripheral setting
 *
 */

#include "F28x_Project.h"           // Device Header File and Examples Include File
#include "defines_cpu1.h"
#include "global_declaration_core1.h"
#include "function_declarations.h"


void spi_init(void)
{
    // ---- SPI-A (LDCs on J7 and 8) ----
    EALLOW;
    GpioCtrlRegs.GPBDIR.bit.GPIO35 = 1;  // Configure GPIO35 as output
    GpioCtrlRegs.GPBDIR.bit.GPIO37 = 1;  // Configure GPIO37 as output

    GpioCtrlRegs.GPBPUD.bit.GPIO58 = 0;     // Enable pull-up on GPIO58 (SPISIMOA)
    GpioCtrlRegs.GPBPUD.bit.GPIO59 = 0;     // Enable pull-up on GPIO59 (SPISOMIA)
    GpioCtrlRegs.GPBPUD.bit.GPIO60 = 0;     // Enable pull-up on GPIO60 (SPICLKA)
    GpioCtrlRegs.GPBPUD.bit.GPIO61 = 0;     // Enable pull-up on GPIO61 (SPISTEA)
    GpioCtrlRegs.GPBPUD.bit.GPIO35 = 0;     // Enable pull-up on GPIO35 (CS1)
    GpioCtrlRegs.GPBPUD.bit.GPIO37 = 0;     // Enable pull-up on GPIO37 (CS2)

    // Select asynchronous qualification
    GpioCtrlRegs.GPBQSEL2.bit.GPIO58 = 3;   // Asynch input GPIO58 (SPISIMOA)
    GpioCtrlRegs.GPBQSEL2.bit.GPIO59 = 3;   // Asynch input GPIO59 (SPISOMIA)
    GpioCtrlRegs.GPBQSEL2.bit.GPIO60 = 3;   // Asynch input GPIO60 (SPICLKA)
    GpioCtrlRegs.GPBQSEL2.bit.GPIO61 = 3;   // Asynch input GPIO61 (SPISTEA)

    // Configure pins (set GPxMUX pins to zero first as per TRM 18.2.2)
    GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 0;
    GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 0;
    GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 0;
    GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 0;
    GpioCtrlRegs.GPBMUX1.bit.GPIO35 = 0;    // Configure GPIO35 as CS1 (GPIO)
    GpioCtrlRegs.GPBMUX1.bit.GPIO37 = 0;    // Configure GPIO37 as CS2 (GPIO)

    GpioCtrlRegs.GPBGMUX2.bit.GPIO58 = 3;
    GpioCtrlRegs.GPBGMUX2.bit.GPIO59 = 3;
    GpioCtrlRegs.GPBGMUX2.bit.GPIO60 = 3;
    GpioCtrlRegs.GPBGMUX2.bit.GPIO61 = 3;
    GpioCtrlRegs.GPBGMUX1.bit.GPIO35 = 0;
    GpioCtrlRegs.GPBGMUX1.bit.GPIO37 = 0;

    GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 3;    // Configure GPIO58 as SPISIMOA
    GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 3;    // Configure GPIO59 as SPISOMIA
    GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 3;    // Configure GPIO60 as SPICLKA
    GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 3;    // Configure GPIO61 as SPISTEA
    EDIS;

    SpiaRegs.SPICCR.bit.SPISWRESET = 0;     // Force the SPI to the reset state

    // Configure SPI-A
    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;       // Master mode
    // Set SPICLK polarity and phase
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 1;        // Falling edge transmit
    SpiaRegs.SPICTL.bit.CLK_PHASE = 0;          // Without delay
//    SpiaRegs.SPIBRR.all = 3;                    // Set baud rate
    SpiaRegs.SPIBRR.all = 39;                       // Set the baud rate (/10)
    SpiaRegs.SPICCR.bit.HS_MODE = 1;            // Enable High Speed Mode
    SpiaRegs.SPICCR.bit.SPICHAR = 7;            // Set SPI character length (SPICHAR=length-1)

    SpiaRegs.SPISTS.bit.OVERRUN_FLAG = 1;       // Clear the overrun flag

    // Configure the FIFO
    SpiaRegs.SPIFFTX.bit.SPIFFENA = 1;
    SpiaRegs.SPIFFTX.bit.TXFIFO = 1;
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 1;
    SpiaRegs.SPIFFCT.bit.TXDLY = 0;

    SpiaRegs.SPICTL.bit.TALK = 1;               // Enable Transmit path

    SpiaRegs.SPICCR.bit.SPISWRESET = 1;         // Release from Reset

    // Disable output on both channels
    GpioDataRegs.GPBCLEAR.bit.GPIO35 = 1;
    GpioDataRegs.GPBCLEAR.bit.GPIO37 = 1;

    //DELAY_US(100);

    // ---- SPI-B (LDCs on J9 and 11) ----
    EALLOW;
    GpioCtrlRegs.GPBDIR.bit.GPIO36 = 1;  // Configure GPIO36 as output
    GpioCtrlRegs.GPBDIR.bit.GPIO38 = 1;  // Configure GPIO38 as output

    GpioCtrlRegs.GPBPUD.bit.GPIO63 = 0;  // Enable pull-up on GPIO63 (SPISIMOB)
    GpioCtrlRegs.GPCPUD.bit.GPIO64 = 0;  // Enable pull-up on GPIO64 (SPISOMIB)
    GpioCtrlRegs.GPCPUD.bit.GPIO65 = 0;  // Enable pull-up on GPIO65 (SPICLKB)
    GpioCtrlRegs.GPCPUD.bit.GPIO66 = 0;  // Enable pull-up on GPIO66 (SPISTEB)
    GpioCtrlRegs.GPBPUD.bit.GPIO36 = 0;  // Enable pull-up on GPIO36 (CS1)
    GpioCtrlRegs.GPBPUD.bit.GPIO38 = 0;  // Enable pull-up on GPIO38 (CS2)

    // Select asynchronous qualification
    GpioCtrlRegs.GPBQSEL2.bit.GPIO63 = 3;
    GpioCtrlRegs.GPCQSEL1.bit.GPIO64 = 3;
    GpioCtrlRegs.GPCQSEL1.bit.GPIO65 = 3;
    GpioCtrlRegs.GPCQSEL1.bit.GPIO66 = 3;

    // Configure pins (set GPxMUX pins to zero first as per TRM 18.2.2)
    GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 0;
    GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 0;
    GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 0;
    GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 0;
    GpioCtrlRegs.GPBMUX1.bit.GPIO36 = 0;
    GpioCtrlRegs.GPBMUX1.bit.GPIO38 = 0;

    GpioCtrlRegs.GPBGMUX2.bit.GPIO63 = 3;
    GpioCtrlRegs.GPCGMUX1.bit.GPIO64 = 3;
    GpioCtrlRegs.GPCGMUX1.bit.GPIO65 = 3;
    GpioCtrlRegs.GPCGMUX1.bit.GPIO66 = 3;
    GpioCtrlRegs.GPBGMUX1.bit.GPIO36 = 0;
    GpioCtrlRegs.GPBGMUX1.bit.GPIO38 = 0;

    GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 3;    // Configure GPIO63 as SPISIMOB
    GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 3;    // Configure GPIO64 as SPISOMIB
    GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 3;    // Configure GPIO65 as SPICLKB
    GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 3;    // Configure GPIO66 as SPISTEB
    GpioCtrlRegs.GPBMUX1.bit.GPIO36 = 0;    // Configure GPIO36 as CS1
    GpioCtrlRegs.GPBMUX1.bit.GPIO38 = 0;    // Configure GPIO38 as CS2
    EDIS;

    SpibRegs.SPICCR.bit.SPISWRESET = 0;     // Force the SPI to the reset state

    // Configure SPI-B
    SpibRegs.SPICTL.bit.MASTER_SLAVE = 1;           // Master mode
    // Set SPICLK polarity and phase
    SpibRegs.SPICCR.bit.CLKPOLARITY = 1;            // Falling edge transmit
    SpibRegs.SPICTL.bit.CLK_PHASE = 0;              // Without delay
    //SpibRegs.SPIBRR.all = 3;                      // Set the baud rate
    SpibRegs.SPIBRR.all = 39;                       // Set the baud rate (/10)
    SpibRegs.SPICCR.bit.HS_MODE = 1;                // Enable High Speed Mode
    SpibRegs.SPICCR.bit.SPICHAR = 7;                // Set the SPI character length

    SpibRegs.SPISTS.bit.OVERRUN_FLAG = 1;           // Clear the overrun flag

    // Configure the FIFO
    SpibRegs.SPIFFTX.bit.SPIFFENA = 1;
    SpibRegs.SPIFFTX.bit.TXFIFO = 1;
    SpibRegs.SPIFFRX.bit.RXFIFORESET = 1;
    SpibRegs.SPIFFCT.bit.TXDLY = 0;

    SpibRegs.SPICTL.bit.TALK = 1;                   // Enable Transmit path

    SpibRegs.SPICCR.bit.SPISWRESET = 1;             // Release from reset

    // Disable output on both channels
    GpioDataRegs.GPBCLEAR.bit.GPIO36 = 1;
    GpioDataRegs.GPBCLEAR.bit.GPIO38 = 1;

    //DELAY_US(100);

    // ---- SPI-C (MA600 on J17) ----
    EALLOW;
    GpioCtrlRegs.GPCDIR.bit.GPIO67 = 1;  // Configure GPIO67 as output
    // Configure GPIO for SPI-C
    GpioCtrlRegs.GPCPUD.bit.GPIO67 = 0;  // Enable pull-up on GPIO67 (CS)
    GpioCtrlRegs.GPCPUD.bit.GPIO69 = 0;  // Enable pull-up on GPIO69 (SPISIMOC)
    GpioCtrlRegs.GPCPUD.bit.GPIO70 = 0;  // Enable pull-up on GPIO70 (SPISOMIC)
    GpioCtrlRegs.GPCPUD.bit.GPIO71 = 0;  // Enable pull-up on GPIO71 (SPICLKC)
    GpioCtrlRegs.GPCPUD.bit.GPIO72 = 0;  // Enable pull-up on GPIO72 (SPISTEC)

    // Select asynchronous qualification
    GpioCtrlRegs.GPCQSEL1.bit.GPIO69 = 3;
    GpioCtrlRegs.GPCQSEL1.bit.GPIO70 = 3;
    GpioCtrlRegs.GPCQSEL1.bit.GPIO71 = 3;
    GpioCtrlRegs.GPCQSEL1.bit.GPIO72 = 3;

    // Configure pins (set GPxMUX pins to zero first as per TRM 18.2.2)
    GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 0;
    GpioCtrlRegs.GPCMUX1.bit.GPIO69 = 0;
    GpioCtrlRegs.GPCMUX1.bit.GPIO70 = 0;
    GpioCtrlRegs.GPCMUX1.bit.GPIO71 = 0;
    GpioCtrlRegs.GPCMUX1.bit.GPIO72 = 0;

    GpioCtrlRegs.GPCGMUX1.bit.GPIO67 = 0;
    GpioCtrlRegs.GPCGMUX1.bit.GPIO69 = 3;
    GpioCtrlRegs.GPCGMUX1.bit.GPIO70 = 3;
    GpioCtrlRegs.GPCGMUX1.bit.GPIO71 = 3;
    GpioCtrlRegs.GPCGMUX1.bit.GPIO72 = 3;

    GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 0;    // Configure GPIO67 as CS
    GpioCtrlRegs.GPCMUX1.bit.GPIO69 = 3;    // Configure GPIO69 as SPISIMOC
    GpioCtrlRegs.GPCMUX1.bit.GPIO70 = 3;    // Configure GPIO70 as SPISOMIC
    GpioCtrlRegs.GPCMUX1.bit.GPIO71 = 3;    // Configure GPIO65 as SPICLKC
    GpioCtrlRegs.GPCMUX1.bit.GPIO72 = 3;    // Configure GPIO66 as SPISTEC
    EDIS;

    SpicRegs.SPICCR.bit.SPISWRESET = 0;     // Force the SPI to the reset state

    // Configure SPI-C
    SpicRegs.SPICTL.bit.MASTER_SLAVE = 1;           // Master mode
    // Set SPICLK polarity and phase
    SpicRegs.SPICCR.bit.CLKPOLARITY = 1;            // Falling edge transmit
    SpicRegs.SPICTL.bit.CLK_PHASE = 0;              // Without delay
    //SpicRegs.SPIBRR.all = 3;                      // Set the baud rate
    SpicRegs.SPIBRR.all = 39;                       // Set the baud rate (/10)
    SpicRegs.SPICCR.bit.HS_MODE = 1;                // Enable High Speed Mode
    SpicRegs.SPICCR.bit.SPICHAR = 15;                // Set the SPI character length
    SpicRegs.SPICTL.bit.OVERRUNINTENA = 0;          // Disable RECEIVER OVERRUN interrupts

    // Configure the FIFO
    SpicRegs.SPIFFTX.bit.SPIFFENA = 1;
    SpicRegs.SPIFFTX.bit.TXFIFO = 1;
    SpicRegs.SPIFFRX.bit.RXFIFORESET = 1;
    SpicRegs.SPIFFCT.bit.TXDLY = 0;

    SpicRegs.SPICTL.bit.TALK = 1;                   // Enable Transmit path

    SpicRegs.SPICCR.bit.SPISWRESET = 1;             // Release from reset

    GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1;           // Disable output

    DELAY_US(100);

}
