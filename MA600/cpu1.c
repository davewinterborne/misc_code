void main(void)
{

    init_SPI();             // Init SPI modules
    GpioDataRegs.GPCSET.bit.GPIO67 = 1;     // Enable SPI-C output driver (MA600 interface)

}

static bool ma600_setup = 0;
// interrupt triggered from EPWM1_INT (PIE)
interrupt void PWM1_ISR(void)
{
    // For some reason I found the MA600 wasn't configured properly if initialised from main
    // Init MA600 on first interrupt only
    if (ma600_setup == 0){
        init_ma600();
        ma600_setup = 1;
}
