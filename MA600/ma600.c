#include "F28x_Project.h"           // Device Header File and Examples Include File
#include "defines_cpu1.h"
#include "global_declaration_core1.h"
#include "function_declarations.h"

void read_ma600(uint16_t* result){
    GpioDataRegs.GPCSET.bit.GPIO67 = 1;     // Enable
    SpicRegs.SPIFFTX.bit.SPIRST = 1;

    // Wait until space is available in Tx FIFO (poll SPIFFTX bits TXFFST(12:8) while it equals 16)
    while (SpicRegs.SPIFFTX.bit.TXFFST == 16){
    }

    SpicRegs.SPITXBUF = 0;      // Write zero
    SpicRegs.SPITXBUF = 0;      // Write dummy zero

    // Wait until all data is sent (Tx FIFO is empty)
        while (SpicRegs.SPIFFTX.bit.TXFFST > 0){
    }

    // Write data from Rx buffer into result
    *(result) = SpicRegs.SPIRXBUF;

    SpicRegs.SPIFFTX.bit.SPIRST = 0;
//    GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1;   // Disable

    return;
}
