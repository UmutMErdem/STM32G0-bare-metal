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

.equ GPIOA_BASE,       (0x50000000)          // GPIOA base address
.equ GPIOB_BASE,       (0x50000400)          // GPIOB base address
.equ GPIOA_MODER,      (GPIOA_BASE + (0x00)) // GPIOA MODER register offset
.equ GPIOB_MODER,      (GPIOB_BASE + (0x00)) // GPIOB MODER register offset
.equ GPIOA_IDR,		   (GPIOA_BASE + (0x10)) // GPIOA IDR register offset
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
	/* enable GPIOA clock, bit2 on IOPENR */
    ldr r6, =RCC_IOPENR
    ldr r2, [r6]
   // Configure the GPIOA for PA pins
    movs r4, 0x3
    orrs r2, r2, r4
    str r2, [r6]


    ldr r0, =GPIOA_MODER
    ldr r2, [r0]
    ldr r4, =0x3
    bics r2, r2, r4
    str r2, [r0]

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
   	ldr r4, =GPIOA_IDR
   	ldr r0, [r4]

leftDirect:
        ldr r2, [r1]
       	lsls r2, #1
       	ldr r4, =0x700
       	cmp r2,r4
       	beq t1
       	ldr r4, =0x608
       	str r2, [r1]
       	cmp r2,r4
       	beq t2
       	ldr r4, =0x418
       	cmp r2,r4
       	beq t3
       	str r2, [r1]
       	ldr r3, =600000
       	bl delay
       	bl button1
       	b leftDirect

t1:
   		ldr r2,= 0x304
   	    str r2, [r1]
   	    ldr r3, =600000
   	    bl delay
   	    bl button1
   	    b leftDirect

t2:
	    ldr r2,= 0x20c
   	    str r2, [r1]
   	    ldr r3, =600000
   	    bl delay
   	    bl button1
   	    b leftDirect
t3:
   		ldr r2,= 0x1c
     	str r2, [r1]
     	ldr r3, =600000
   	    bl delay
   	    bl button1
   	    b leftDirect
RightDirect:
        ldr r2, [r1]
       	lsrs r2, #1
       	ldr r4, =0xe
       	cmp r2,r4
       	beq t4
       	ldr r4, =0x106
       	str r2, [r1]
       	cmp r2,r4
       	beq t5
       	ldr r4, =0x182
       	cmp r2,r4
       	beq t6
       	str r2, [r1]
       	ldr r3, =600000
       	bl delay
       	bl button2
       	b  RightDirect
t4:
   		ldr r2,= 0x20c
   	    str r2, [r1]
   	    ldr r3, =600000
   	    bl delay
   	    bl button2
   	    b  RightDirect
t5:
	    ldr r2,= 0x304
   	    str r2, [r1]
   	    ldr r3, =600000
   	    bl delay
   	    bl button2
   	    b  RightDirect
t6:
   		ldr r2,= 0x380
     	str r2, [r1]
     	ldr r3, =600000
   	    bl delay
   	    bl button2
   	    b  RightDirect

button1:
        ldr r4, =GPIOA_IDR
        ldr r5, [r4]
        cmp r0 , r5
 		bne RightDirect
		bx lr

button2:
		ldr r4, =GPIOA_IDR
        ldr r5, [r4]
        cmp r0 , r5
 		beq leftDirect
		bx lr

delay:
    	subs r3, #1 // r1 = r1 - 1
		bne delay // r1 is not equal to 0.
		bx lr // r1 is equal to 0.
	/* for(;;); */

b .

