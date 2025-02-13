#include "F28x_Project.h"           // Device Header File and Examples Include File
#include "defines_cpu1.h"
#include "global_declaration_core1.h"
#include "function_declarations.h"

// An example of setting up bias current trimming (BCT) on the MPS MA600 magentic angle sensor chip
// This functionality scales one of the two measured axes to compensate for different magnetic field strengths,
// thus eliminating the second harmonic from the measured angle waveform
// In this case, the MA600 is using SPI-C, which is configured in init_SPI.c
// Each register setting requires 3 16bit words to be transmitted, with the CS line pulled high for a minimum of 120ns in between:
// 1. The write command
// 2. The register address and value to write
// 3. A zero word

void init_ma600(void){
    // Set the BCT value (calculate this as per the MA600 datasheet, or by trial and error)
    SpicRegs.SPIFFTX.bit.SPIRST = 1;            // Release from reset
    while (SpicRegs.SPIFFTX.bit.TXFFST == 16){
    }
    SpicRegs.SPITXBUF = 0b1110101001010100;     // Write write command (tells the MA600 we want to write to a register)
    SpicRegs.SPITXBUF = 0;                      // Write dummy zero
    while (SpicRegs.SPIFFTX.bit.TXFFST > 0){    // Wait until all data is sent (Tx FIFO is empty)
    }
    SpicRegs.SPIFFTX.bit.SPIRST = 0;            // Reset

    SpicRegs.SPIFFTX.bit.SPIRST = 1;            // Release from reset
    while (SpicRegs.SPIFFTX.bit.TXFFST == 16){
    }
    SpicRegs.SPITXBUF = (2 << 8) | 40;          // Write BCT value to register address 2 (register address in the MSB, value to write in the LSB)
    SpicRegs.SPITXBUF = 0;                      // Write dummy zero
    while (SpicRegs.SPIFFTX.bit.TXFFST > 0){    // Wait until all data is sent (Tx FIFO is empty)
    }
    SpicRegs.SPIFFTX.bit.SPIRST = 0;            // Reset

    SpicRegs.SPIFFTX.bit.SPIRST = 1;            // Release from reset
    while (SpicRegs.SPIFFTX.bit.TXFFST == 16){
    }
    SpicRegs.SPITXBUF = 0;                      // Write zero
    SpicRegs.SPITXBUF = 0;                      // Write dummy zero
    while (SpicRegs.SPIFFTX.bit.TXFFST > 0){    // Wait until all data is sent (Tx FIFO is empty)
    }
    SpicRegs.SPIFFTX.bit.SPIRST = 0;            // Reset

    bct_value = SpicRegs.SPIRXBUF;              // Check received value - the MA600 echoes the value written back in the LSB (the MSB contains the 8 bit angle result)

    // Enable BCT in one axis
    SpicRegs.SPIFFTX.bit.SPIRST = 1;            // Release from reset
    while (SpicRegs.SPIFFTX.bit.TXFFST == 16){
    }
    SpicRegs.SPITXBUF = 0b1110101001010100;     // Write write command
    SpicRegs.SPITXBUF = 0;                      // Write dummy zero
    while (SpicRegs.SPIFFTX.bit.TXFFST > 0){    // Wait until all data is sent (Tx FIFO is empty)
    }
    SpicRegs.SPIFFTX.bit.SPIRST = 0;            // Reset

    SpicRegs.SPIFFTX.bit.SPIRST = 1;            // Release from reset
    while (SpicRegs.SPIFFTX.bit.TXFFST == 16){
    }
    SpicRegs.SPITXBUF = (3 << 8) | 2;           // Write 2 to register address 3
	// In this case we set	bit 1 (ETY), enabling BCT in the Y axis - writing 1 will set bit 0 (ETX) enabling BCT in the X axis. Don't enable both, or it will have no effect!
    SpicRegs.SPITXBUF = 0;                      // Write dummy zero
    while (SpicRegs.SPIFFTX.bit.TXFFST > 0){    // Wait until all data is sent (Tx FIFO is empty)
    }
    SpicRegs.SPIFFTX.bit.SPIRST = 0;            // Reset

    SpicRegs.SPIFFTX.bit.SPIRST = 1;            // Release from reset
    while (SpicRegs.SPIFFTX.bit.TXFFST == 16){
    }
    SpicRegs.SPITXBUF = 0;                      // Write zero
    SpicRegs.SPITXBUF = 0;                      // Write dummy zero
    while (SpicRegs.SPIFFTX.bit.TXFFST > 0){    // Wait until all data is sent (Tx FIFO is empty)
    }
    SpicRegs.SPIFFTX.bit.SPIRST = 0;            // Reset

    bct_x_enable= SpicRegs.SPIRXBUF;            // Check received value - the MA600 echoes the value written back in the LSB (the MSB contains the 8 bit angle result)

    return;
}
