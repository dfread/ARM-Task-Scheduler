/* Master's Project - Task Scheduler w/ Priority switch function */
/* Daniel Fread, Binghamton University */

#include <MKL25Z4.h>
#include <stdint.h>
#include "rgbled.h"
#include "uart0_irq.h"
#include "uclib.h"
#include <stdbool.h>

void task0(void);
void task1(void);
void task2(void);

#define STACK_SIZE 1024
#define NUM_TASKS 3
#define DELAY 99999
//Priority levels also translate to how many time slices each task will be allocated
//AKA: Priority level 3 gets 3 time slices before being swapped out etc..
#define TASK0_PRIO 5
#define TASK1_PRIO 10
#define TASK2_PRIO 20

//Additional data can be initialized in the stack, which can be placed after the stack pointer
//	We only have 1 custom stack value, which is setting default priority levels for each task
//	More can be added by placing them between STACK_PTR_LOC and FLAG_LOC
//  FLAG_LOC and RETURN_ADDR_LOC must be placed after, these defines automatically move them after your custom data
//	Update NUM_STACK_PROPERTIES to do this
#define NUM_STACK_PROPERTIES 1 //Use for custom stack values; in this example, we only have 1
#define STACK_PTR_LOC (STACK_SIZE-1)
#define DEFAULT_PRIO_LOC (STACK_SIZE-2)
//For example: Adding additional data 'X' can use X_LOC (STACK_SIZE-3)
//	The defines below should adjust accordingly if NUM_STACK_PROPERTIES is updated
#define FLAG_LOC (STACK_SIZE-(NUM_STACK_PROPERTIES+2))
#define RETURN_ADDR_LOC (STACK_SIZE-(NUM_STACK_PROPERTIES+3))
#define FIRST_SP_LOC (STACK_SIZE-(NUM_STACK_PROPERTIES+9))

_Bool first_Svc_hit = true;

unsigned task_stack[NUM_TASKS][STACK_SIZE];
unsigned task_number = 2; //Scheduler begins as if transitioning to new task, so start at last task to start at task 0

void main()
{
	SIM->COPC = 0;
	initialize_rgbled();
	configure_uart0();
	print_str("\n\r");

	//Seeding stack w/ initial values of registers as if its been interrupted
	task_stack[0][STACK_PTR_LOC] = (unsigned)&task_stack[0][FIRST_SP_LOC]; //Placing stack pointer on entry at bottom of stack
	task_stack[0][DEFAULT_PRIO_LOC] = TASK0_PRIO; //Time slices for task 0
	task_stack[0][FLAG_LOC] = 0x01000000;
	task_stack[0][RETURN_ADDR_LOC] = (void *)&task0;

	task_stack[1][STACK_PTR_LOC] = (unsigned)&task_stack[1][FIRST_SP_LOC];
	task_stack[1][DEFAULT_PRIO_LOC] = TASK1_PRIO; //Time slices for task 1
	task_stack[1][FLAG_LOC] = 0x01000000;
	task_stack[1][RETURN_ADDR_LOC] = (void *)&task1;

	task_stack[2][STACK_PTR_LOC] = (unsigned)&task_stack[2][FIRST_SP_LOC];
	task_stack[2][DEFAULT_PRIO_LOC] = TASK2_PRIO; //Time slices for task 2
	task_stack[2][FLAG_LOC] = 0x01000000;
	task_stack[2][RETURN_ADDR_LOC] = (void *)&task2;

	asm("CPSID I");
	SysTick->LOAD = 16000000; //each task per ms
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
					SysTick_CTRL_TICKINT_Msk |
					SysTick_CTRL_ENABLE_Msk;

	NVIC_SetPriority(PendSV_IRQn, 0x3);
	NVIC_SetPriority(SysTick_IRQn, 0x1);
	asm("CPSIE I");
	asm("SVC #0");

}
void task0(void)
{
	uint32_t cnt = 0;
	asm("SVC #14");
	while(1)
	{
		cnt = cnt == DELAY ? 0 : cnt+1;
		if( cnt == 0)
			toggle_green_led();
		turn_off_blue_led();
		turn_off_red_led();
	}
}
void task1(void)
{
	uint32_t cnt = 0;
	asm("SVC #7");
	while(1)
	{
		cnt = cnt == DELAY? 0 : cnt+1;
		if( cnt == 0)
			toggle_blue_led();
		turn_off_green_led();
		turn_off_red_led();
	}
}
void task2(void)
{
	uint32_t cnt = 0;
	asm("SVC #8");
	while(1)
	{
		cnt = cnt == DELAY ? 0 : cnt+1;
		if( cnt == 0)
			toggle_red_led();
		turn_off_green_led();
		turn_off_blue_led();
	}
}

__attribute__ ((naked)) void PendSV_Handler()
{
	static uint8_t num_slices = 1;

	asm("CPSID I"); //low priority so we don't want to be interrupted

	if(--num_slices){
		print_hex8(num_slices);
		print_str("\n\r");
		goto END;
	}

	//save process context
	asm("mrs r0,psp"); 
	asm("sub r0,#32"); 
	asm("stm r0!,{r4,r5,r6,r7}"); 
	asm("mov r4,r8");
	asm("mov r5,r9");
	asm("mov r6,r10");
	asm("mov r7,r11");
	asm("stm r0!,{r4,r5,r6,r7}");

	//note this sequence will dirty registers and that's okay
	//save this process stack
	task_stack[task_number][STACK_PTR_LOC] = __get_PSP();

	//schedule next task
	task_number = (task_number >= NUM_TASKS-1) ? 0:task_number+1;

	//Load priority level/time slice amount
	num_slices = task_stack[task_number][DEFAULT_PRIO_LOC];

	print_hex8(num_slices);
	print_str("\n\r");

	//get task SP
	__set_PSP(task_stack[task_number][STACK_PTR_LOC]); 

	//restore next context
	asm("mrs r0,psp"); 
	asm("sub r0,#16"); 
	asm("ldm r0!,{R4,R5,R6,R7}");
	asm("mov r8,r4");
	asm("mov r9,r5");
	asm("mov r10,r6");
	asm("mov r11,r7");
	asm("sub r0,#32");
	asm("ldm r0!,{r4,r5,r6,r7}");

END:
	asm("CPSIE I");
	//we need to return from ISR without any "dressing" from C
	asm("ldr r0,=0xfffffffd");
	asm("bx r0");
}

void SysTick_Handler() 
{
	//trigger PENDSV
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void SVC_Handler() //Go here upon program start, and only if a task wants to change its priority level
{
	if(first_Svc_hit){
		//Unprivileged Mode (not all registers can be accessed)
		__set_CONTROL(0x03);
		//start first task
		__set_PSP(task_stack[task_number][STACK_PTR_LOC]); 
		//trigger PENDSV
		SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
		first_Svc_hit = false;
	}
	else{ //Priority switch function call
		uint8_t new_prio = 0;
		uint32_t *SP_reg;
		uint16_t *PC_reg;
		asm("MRS %0,PSP":"=r"(SP_reg)); //grabs PSP of current task and places it into SP_reg
		PC_reg = SP_reg[6]; //SVC instruction is located one word before program counter in stack, which is 6 words down from PSP
		new_prio = PC_reg[-1] & 0xFF; //The argument for the priority change is in the least significant 8 bits
		task_stack[task_number][DEFAULT_PRIO_LOC] = new_prio; //Replace original value in stack with requested priority level
		print_str("Task "); print_hex8(task_number); print_str(" just changed it's priority level to "); print_hex8((new_prio)); print_str("\n\r");
	}
}
