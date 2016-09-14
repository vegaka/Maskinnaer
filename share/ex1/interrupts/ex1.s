.syntax unified

.include "efm32gg.s"

/////////////////////////////////////////////////////////////////////////////
//
// Exception vector table
// This table contains addresses for all exception handlers
//
/////////////////////////////////////////////////////////////////////////////

.section .vectors

	.long   stack_top               /* Top of Stack                 */
	.long   _reset                  /* Reset Handler                */
	.long   dummy_handler           /* NMI Handler                  */
	.long   dummy_handler           /* Hard Fault Handler           */
	.long   dummy_handler           /* MPU Fault Handler            */
	.long   dummy_handler           /* Bus Fault Handler            */
	.long   dummy_handler           /* Usage Fault Handler          */
	.long   dummy_handler           /* Reserved                     */
	.long   dummy_handler           /* Reserved                     */
	.long   dummy_handler           /* Reserved                     */
	.long   dummy_handler           /* Reserved                     */
	.long   dummy_handler           /* SVCall Handler               */
	.long   dummy_handler           /* Debug Monitor Handler        */
	.long   dummy_handler           /* Reserved                     */
	.long   dummy_handler           /* PendSV Handler               */
	.long   dummy_handler           /* SysTick Handler              */

	/* External Interrupts */
	.long   dummy_handler
	.long   gpio_handler            /* GPIO even handler */
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   gpio_handler            /* GPIO odd handler */
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler
	.long   dummy_handler

.section .text

/////////////////////////////////////////////////////////////////////////////
//
// Reset handler
// The CPU will start executing here after a reset
//
/////////////////////////////////////////////////////////////////////////////

	.globl  _reset
	.type   _reset, %function
	.thumb_func
_reset:
	/* Enable GPIO clock */
	ldr r1, =CMU_BASE
	ldr r2, [r1, #CMU_HFPERCLKEN0]
	mov r3, #1
	lsl r3, r3, #CMU_HFPERCLKEN0_GPIO
	orr r2, r2, r3
	str r2, [r1, #CMU_HFPERCLKEN0]
	
	/* Set GPIO A drive strength */
	mov r1, 0x2
	ldr r2, =GPIO_PA_BASE
	str r1, [r2, #GPIO_CTRL]
	
	/* Enable output on pin 8-15 on GPIO A */
	mov r1, 0x55555555
	ldr r2, =GPIO_PA_BASE
	str r1, [r2, #GPIO_MODEH]
	
	/* Enable input on pin 0-7 on GPIO C */
	mov r1, 0x33333333
	ldr r2, =GPIO_PC_BASE
	str r1, [r2, #GPIO_MODEL]

	/* Enable internal pull-up on GPIO C */
	mov r0, 0xff
	str r0, [r2, #GPIO_DOUT]

	/* Enable GPIO interrupts */
	mov r0, 0x22222222
	ldr r1, =GPIO_BASE
	str r0, [r1, #GPIO_EXTIPSELL]
	mov r2, 0xff
	str r2, [r1, #GPIO_EXTIFALL]
	str r2, [r1, #GPIO_EXTIRISE]
	str r2, [r1, #GPIO_IEN]
	ldr r3, =0x802
	ldr r4, =ISER0
	str r3, [r4]

	wfi

waitForInterrupt:
	wfi
	b waitForInterrupt

/////////////////////////////////////////////////////////////////////////////
//
// GPIO handler
// The CPU will jump here when there is a GPIO interrupt
//
/////////////////////////////////////////////////////////////////////////////

    .thumb_func
gpio_handler:
	/* Reset lights */
	mov r1, 0xff00
	ldr r2, =GPIO_PA_BASE
	ldr r3, [r2, #GPIO_DOUT]
	orr r3, r1, r3
	str r3, [r2, #GPIO_DOUT]

	/* Check button states */
	ldr r0, =GPIO_PC_BASE
	ldr r1, [r0, #GPIO_DIN]
	lsl r1, r1, #8
	
	/* Turn on lights according to buttons pressed */
	ldr r4, =GPIO_PA_BASE
	ldr r5, [r4, #GPIO_DOUT]
	and r5, r5, r1
	str r5, [r4, #GPIO_DOUT]

	/* Cleanup */
	ldr r1, =GPIO_BASE
	ldr r2, [r1, #GPIO_IF]
	str r2, [r1, #GPIO_IFC]
	bx lr

/////////////////////////////////////////////////////////////////////////////

    .thumb_func
dummy_handler:  
	b .  // do nothing

