/* Baremetal startup code for the FRDM-KL25Z board
 * Douglas Summerville, Binghamton University, 2018
 */
#include "MKL25Z4.h"

//Segment Locations Declared in Linker Script
extern uint32_t __stacktop[];
extern uint32_t __data[], __edata[];
extern uint32_t __bss_start[], __bss_end[];
extern uint32_t __etext[];

//Signature of application entry point
extern void main( void );

//Signature of function pointed to by reset vector
void _reset_init(void)    __attribute__((naked, aligned(8)));

// Flash Configuration Field Values
// WARNING: Do not edit these or you can brick your device
const uint8_t _cfm[0x10] __attribute__ ((section (".flash_configuration_field"))) = 
{
    //8B backdoor comparison key
    0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,  0xFF,

    //Flash Protection Bytes (DO NOT ALTER)
    0xFF,  0xFF,  0xFF,  0xFF,

    /*Security Options
	[7:6] Backdoor key access disabled 0b01
	[5:4] Mass Erase Enabled 0b11
	[3:2] Freescale Failure Analysis Access Granted 0b11
	[1:0] MCU Security Unsecured 0b10
    */
    0x7E,

    /*Boot Options
	[7,6,1] Reserved 0b111
	[5] Fast Initialization 0b1
	[3] Reset Pin (controlled by ifdef RESET_PIN_DISABLE)
	[2] NMI Enabled 1b1
	[4,1] Boot clock divide-by-1 0b11
     */
#ifdef RESET_PIN_DISABLE
    0xF7,
#else
    0xFF,
#endif
    
    //Reserved
    0xFF, 0xFF
  };

// Initialize system clock to 48 Mhz
static void __attribute__ ((section (".system_startup_code"))) init_clocks(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x01) | SIM_CLKDIV1_OUTDIV4(0x01);
    SIM->SOPT1 &= ~SIM_SOPT1_OSC32KSEL(0x03);
    SIM->SOPT2 = (SIM->SOPT2 &~ (SIM_SOPT2_TPMSRC_MASK)) | SIM_SOPT2_TPMSRC(0x01)|SIM_SOPT2_PLLFLLSEL_MASK;
    PORTA->PCR[18] &= ~((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
    PORTA->PCR[19] &= ~((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
    OSC0->CR = 0;
    MCG->C2 = (MCG_C2_RANGE0(0x02) | MCG_C2_EREFS0_MASK);
    MCG->C1 = (MCG_C1_CLKS(0x02) | MCG_C1_FRDIV(0x03));
    MCG->C4 &= ~((MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x03)));
    MCG->C5 = MCG_C5_PRDIV0(0x01);
    MCG->C6 = 0;
    while((MCG->S & MCG_S_IREFST_MASK) != 0);
    while((MCG->S & MCG_S_CLKST_MASK) != 8);
    MCG->C6 = MCG_C6_PLLS_MASK;
    while((MCG->S & MCG_S_LOCK0_MASK) == 0);
    MCG->C1 = MCG_C1_FRDIV(0x03);
    while((MCG->S & MCG_S_CLKST_MASK) != 0x0CU);
}

//The default ISR to handle errant interrupts
//Just block forever to prevent further damage
void __attribute__((naked,noreturn,section (".system_startup_code")))  Default_Handler() 
{
    while(1);//loop forever to "stop" processor
}
//The default Handler for Hard Faults
//Just block forever to prevent further damage
void __attribute__((naked,noreturn,section (".system_startup_code")))  HardFault_Handler() 
{
    while(1);//loop forever to "stop" processor
}

/* Weak definitions of handlers point to Default_Handler if not implemented */
void NMI_Handler()          __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void SVC_Handler()          __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void PendSV_Handler()       __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void SysTick_Handler()      __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void DMA0_IRQHandler()      __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void DMA1_IRQHandler()      __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void DMA2_IRQHandler()      __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void DMA3_IRQHandler()      __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void MCM_IRQHandler()       __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void FTFA_IRQHandler()      __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void PMC_IRQHandler()       __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void LLW_IRQHandler()       __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void I2C0_IRQHandler()      __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void I2C1_IRQHandler()      __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void SPI0_IRQHandler()      __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void SPI1_IRQHandler()      __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void UART0_IRQHandler()     __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void UART1_IRQHandler()     __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void UART2_IRQHandler()     __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void ADC0_IRQHandler()      __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void CMP0_IRQHandler()      __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void FTM0_IRQHandler()      __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void FTM1_IRQHandler()      __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void FTM2_IRQHandler()      __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void RTC_Alarm_IRQHandler() __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void RTC_Seconds_IRQHandler() __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void PIT_IRQHandler()       __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void USBOTG_IRQHandler()    __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void DAC0_IRQHandler()      __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void TSI0_IRQHandler()      __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void MCG_IRQHandler()       __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void LPTimer_IRQHandler()   __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void PORTA_IRQHandler()     __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));
void PORTD_IRQHandler()     __attribute__ ((weak, naked, noreturn, alias("Default_Handler")));

// Interrupt vector table: array of pointers to functions 
#define RESERVED Default_Handler
void (* const InterruptVector[])() __attribute__ ((section(".interrupt_vectors"))) = 
{
    (void(*)(void)) __stacktop,                     // Initial stack pointer
    _reset_init,                                    // Reset handler
    NMI_Handler,
    HardFault_Handler,
    RESERVED,
    RESERVED,
    RESERVED,
    RESERVED,
    RESERVED,
    RESERVED,
    RESERVED,
    SVC_Handler,
    RESERVED,
    RESERVED,
    PendSV_Handler,
    SysTick_Handler,
    DMA0_IRQHandler, /* DMA Channel 0 Transfer Complete and Error */
    DMA1_IRQHandler, /* DMA Channel 1 Transfer Complete and Error */
    DMA2_IRQHandler, /* DMA Channel 2 Transfer Complete and Error */
    DMA3_IRQHandler, /* DMA Channel 3 Transfer Complete and Error */
    RESERVED ,
    FTFA_IRQHandler, /* FTFL Interrupt */
    PMC_IRQHandler, /* PMC Interrupt */
    LLW_IRQHandler, /* Low Leakage Wake-up */
    I2C0_IRQHandler, /* I2C0 interrupt */
    I2C1_IRQHandler, /* I2C1 interrupt */
    SPI0_IRQHandler, /* SPI0 Interrupt */
    SPI1_IRQHandler, /* SPI1 Interrupt */
    UART0_IRQHandler, /* UART0 Status and Error interrupt */
    UART1_IRQHandler, /* UART1 Status and Error interrupt */
    UART2_IRQHandler, /* UART2 Status and Error interrupt */
    ADC0_IRQHandler, /* ADC0 interrupt */
    CMP0_IRQHandler, /* CMP0 interrupt */
    FTM0_IRQHandler, /* FTM0 fault, overflow and channels interrupt */
    FTM1_IRQHandler, /* FTM1 fault, overflow and channels interrupt */
    FTM2_IRQHandler, /* FTM2 fault, overflow and channels interrupt */
    RTC_Alarm_IRQHandler, /* RTC Alarm interrupt */
    RTC_Seconds_IRQHandler, /* RTC Seconds interrupt */
    PIT_IRQHandler, /* PIT timer all channels interrupt */
    RESERVED, /* Reserved interrupt 39/23 */
    USBOTG_IRQHandler, /* USB interrupt */
    DAC0_IRQHandler, /* DAC0 interrupt */
    TSI0_IRQHandler, /* TSI0 Interrupt */
    MCG_IRQHandler, /* MCG Interrupt */
    LPTimer_IRQHandler, /* LPTimer interrupt */
    RESERVED, /* Reserved interrupt 45/29 */
    PORTA_IRQHandler, /* Port A interrupt */
    PORTD_IRQHandler /* Port D interrupt */
};

void __attribute__ ((section (".system_startup_code"))) _stop_init(void)
{
#define STOP_PIN_SHIFT 1
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	//PORTE->PCR[STOP_PIN_SHIFT] &= ~PORT_PCR_MUX_MASK;
	//PORTE->PCR[STOP_PIN_SHIFT] |= PORT_PCR_MUX(1);
	PORTE->PCR[STOP_PIN_SHIFT] = PORT_PCR_MUX(1) | PORT_PCR_PE(1);
	PTE->PDDR &= ~(1UL<<STOP_PIN_SHIFT);
        //PORTE->PCR[STOP_PIN_SHIFT] |= PORT_PCR_PE_MASK;
	//Wait while PTE1 held low, feeding WDT
	while(!(PTE->PDIR & (1UL<<STOP_PIN_SHIFT)))
	{
		SIM->SRVCOP=0x55;
		SIM->SRVCOP=0xAA;
	};
	SIM->SCGC5 &= ~SIM_SCGC5_PORTE_MASK;
}

void __attribute__ ((section (".system_startup_code"))) _reset_init(void)
{
	_stop_init();
//The COP is enabled by default and SIM->COPC can be written only once
//If disabled here, it cannot be reenabled in SW
#ifdef WATCHDOG_DISABLE
    SIM->COPC = 0;  
#endif
    SCB->VTOR = (uint32_t)InterruptVector;
    init_clocks();

    // C language DATA segment contains initialized values that must be 
    // copied from FLASH to RAM
    {
	uint32_t *fr = __etext;
	uint32_t *to = __data;
	while( to < __edata )
		*to++ = *fr++;
    //C Language BSS must be cleared
	while( to <= __bss_end )
		*to++ = 0;
    }
    main();
}
