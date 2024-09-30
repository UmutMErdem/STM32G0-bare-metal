.syntax unified
.cpu cortex-m0plus
.fpu softvfp
.thumb


/* make linker see this */
.global Reset_Handler

/* get these from linker script */
.word _sdata
.word _edata
.word _sbss
.word _ebss


/* define peripheral addresses from RM0444 page 57, Tables 3-4 */
.equ RCC_BASE,         (0x40021000)          // RCC base address
.equ RCC_IOPENR,       (RCC_BASE   + (0x34)) // RCC IOPENR register offset

.equ GPIOB_BASE,       (0x50000400)          // GPIOB base address
.equ GPIOB_MODER,      (GPIOB_BASE + (0x00)) // GPIOB MODER register offset
.equ GPIOB_ODR,        (GPIOB_BASE + (0x14)) // GPIOB ODR register offset

/* vector table, +1 thumb mode */
.section .vectors
vector_table:
	.word _estack             /*     Stack pointer */
	.word Reset_Handler +1    /*     Reset handler */
	.word Default_Handler +1  /*       NMI handler */
	.word Default_Handler +1  /* HardFault handler */
	/* add rest of them here if needed */


/* reset handler */
.section .text
Reset_Handler:
	/* set stack pointer */
	ldr r0, =_estack
	mov sp, r0

	/* initialize data and bss
	 * not necessary for rom only code
	 * */
	bl init_data
	/* call main */
	bl main
	/* trap if returned */
	b .


/* initialize data and bss sections */
.section .text
init_data:

	/* copy rom to ram */
	ldr r0, =_sdata
	ldr r1, =_edata
	ldr r2, =_sidata
	movs r3, #0
	b LoopCopyDataInit

	CopyDataInit:
		ldr r4, [r2, r3]
		str r4, [r0, r3]
		adds r3, r3, #4

	LoopCopyDataInit:
		adds r4, r0, r3
		cmp r4, r1
		bcc CopyDataInit

	/* zero bss */
	ldr r2, =_sbss
	ldr r4, =_ebss
	movs r3, #0
	b LoopFillZerobss

	FillZerobss:
		str  r3, [r2]
		adds r2, r2, #4

	LoopFillZerobss:
		cmp r2, r4
		bcc FillZerobss

	bx lr


/* default handler */
.section .text
Default_Handler:
	b Default_Handler


/* main function */
.section .text
main:
	/* enable GPIOB clock on IOPENR */
    ldr r6, =RCC_IOPENR
    ldr r2, [r6]
   // Configure the GPIOB for PB pins
    movs r4, 0x2
    orrs r2, r2, r4
    str r2, [r6]

    ldr r1, =GPIOB_MODER
    ldr r2, [r1]
    ldr r4, =0xFFFF0
    bics r2, r2, r4
    ldr r4, =0x55550
	orrs r2, r2, r4
    str r2, [r1]
	ldr r1, =GPIOB_ODR
    ldr r2,= 0x1c
    str r2, [r1]
    ldr r3, =600000
   	bl delay

leftDirect:
        ldr r2, [r1]
       	lsls r2, #1
       	ldr r4, =0x380
       	str r2, [r1]
       	ldr r3, =600000
       	bl delay
       	cmp r2, r4
       	beq RightDirect
       	b  leftDirect

RightDirect:
        ldr r2, [r1]
       	lsrs r2, #1
       	ldr r4, =0x1c
       	str r2, [r1]
       	ldr r3, =600000
       	bl delay
       	cmp r2,r4
       	beq leftDirect
       	b  RightDirect


delay:
    	subs r3, #1 // r1 = r1 - 1
		bne delay // r1 is not equal to 0.
		bx lr // r1 is equal to 0.
	/* for(;;); */

b .

