
;CodeVisionAVR C Compiler V3.12 Advanced
;(C) Copyright 1998-2014 Pavel Haiduc, HP InfoTech s.r.l.
;http://www.hpinfotech.com

;Build configuration    : Debug
;Chip type              : ATtiny10
;Program type           : Application
;Clock frequency        : 8,000000 MHz
;Memory model           : Tiny
;Optimize for           : Speed
;(s)printf features     : int, width
;(s)scanf features      : int, width
;External RAM size      : 0
;Data Stack size        : 8 byte(s)
;Heap size              : 0 byte(s)
;Promote 'char' to 'int': Yes
;'char' is unsigned     : Yes
;8 bit enums            : Yes
;Global 'const' stored in FLASH: No
;Enhanced function parameter passing: Yes
;'auto' var. watch in AVR Studio: Off

	#define _MODEL_TINY_

	#pragma AVRPART ADMIN PART_NAME ATtiny10
	#pragma AVRPART CORE CORE_VERSION AVR8L_0
	#pragma AVRPART MEMORY PROG_FLASH 1024
	#pragma AVRPART MEMORY EEPROM 0
	#pragma AVRPART MEMORY INT_SRAM SIZE 32
	#pragma AVRPART MEMORY INT_SRAM START_ADDR 0x40

	.LISTMAC
	.EQU WDTCSR=0x31
	.EQU SMCR=0x3A
	.EQU RSTFLR=0x3B
	.EQU CCP=0x3C
	.EQU SPL=0x3D
	.EQU SPH=0x3E
	.EQU SREG=0x3F

	.DEF R0X10=R16
	.DEF R0X11=R17
	.DEF R0X12=R18
	.DEF R0X13=R19
	.DEF R0X14=R20
	.DEF R0X15=R21
	.DEF R0X16=R22
	.DEF R0X17=R23
	.DEF R0X18=R24
	.DEF R0X19=R25
	.DEF R0X1A=R26
	.DEF R0X1B=R27
	.DEF R0X1C=R28
	.DEF R0X1D=R29
	.DEF R0X1E=R30
	.DEF R0X1F=R31

	.EQU __SRAM_START=0x0040
	.EQU __SRAM_END=0x005F
	.EQU __DSTACK_SIZE=0x0008
	.EQU __HEAP_SIZE=0x0000
	.EQU __CLEAR_SRAM_SIZE=__SRAM_END-__SRAM_START+1

	.MACRO __CPD1N
	CPI  R30,LOW(@0)
	LDI  R26,HIGH(@0)
	CPC  R31,R26
	LDI  R26,BYTE3(@0)
	CPC  R22,R26
	LDI  R26,BYTE4(@0)
	CPC  R23,R26
	.ENDM

	.MACRO __CPD2N
	CPI  R26,LOW(@0)
	LDI  R30,HIGH(@0)
	CPC  R27,R30
	LDI  R30,BYTE3(@0)
	CPC  R24,R30
	LDI  R30,BYTE4(@0)
	CPC  R25,R30
	.ENDM

	.MACRO __CPWRR
	CP   R@0,R@2
	CPC  R@1,R@3
	.ENDM

	.MACRO __CPWRN
	CPI  R@0,LOW(@2)
	LDI  R30,HIGH(@2)
	CPC  R@1,R30
	.ENDM

	.MACRO __ADDB1MN
	SUBI R30,LOW(-@0-(@1))
	.ENDM

	.MACRO __ADDB2MN
	SUBI R26,LOW(-@0-(@1))
	.ENDM

	.MACRO __ADDW1MN
	SUBI R30,LOW(-@0-(@1))
	SBCI R31,HIGH(-@0-(@1))
	.ENDM

	.MACRO __ADDW2MN
	SUBI R26,LOW(-@0-(@1))
	SBCI R27,HIGH(-@0-(@1))
	.ENDM

	.MACRO __ADDW1FN
	SUBI R30,LOW(-2*@0-(@1))
	SBCI R31,HIGH(-2*@0-(@1))
	.ENDM

	.MACRO __ADDD1FN
	SUBI R30,LOW(-2*@0-(@1))
	SBCI R31,HIGH(-2*@0-(@1))
	SBCI R22,BYTE3(-2*@0-(@1))
	.ENDM

	.MACRO __ADDD1N
	SUBI R30,LOW(-@0)
	SBCI R31,HIGH(-@0)
	SBCI R22,BYTE3(-@0)
	SBCI R23,BYTE4(-@0)
	.ENDM

	.MACRO __ADDD2N
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	SBCI R24,BYTE3(-@0)
	SBCI R25,BYTE4(-@0)
	.ENDM

	.MACRO __SUBD1N
	SUBI R30,LOW(@0)
	SBCI R31,HIGH(@0)
	SBCI R22,BYTE3(@0)
	SBCI R23,BYTE4(@0)
	.ENDM

	.MACRO __SUBD2N
	SUBI R26,LOW(@0)
	SBCI R27,HIGH(@0)
	SBCI R24,BYTE3(@0)
	SBCI R25,BYTE4(@0)
	.ENDM

	.MACRO __ANDBMNN
	LDS  R30,@0+(@1)
	ANDI R30,LOW(@2)
	STS  @0+(@1),R30
	.ENDM

	.MACRO __ANDWMNN
	LDS  R30,@0+(@1)
	ANDI R30,LOW(@2)
	STS  @0+(@1),R30
	LDS  R30,@0+(@1)+1
	ANDI R30,HIGH(@2)
	STS  @0+(@1)+1,R30
	.ENDM

	.MACRO __ANDD1N
	ANDI R30,LOW(@0)
	ANDI R31,HIGH(@0)
	ANDI R22,BYTE3(@0)
	ANDI R23,BYTE4(@0)
	.ENDM

	.MACRO __ANDD2N
	ANDI R26,LOW(@0)
	ANDI R27,HIGH(@0)
	ANDI R24,BYTE3(@0)
	ANDI R25,BYTE4(@0)
	.ENDM

	.MACRO __ORBMNN
	LDS  R30,@0+(@1)
	ORI  R30,LOW(@2)
	STS  @0+(@1),R30
	.ENDM

	.MACRO __ORWMNN
	LDS  R30,@0+(@1)
	ORI  R30,LOW(@2)
	STS  @0+(@1),R30
	LDS  R30,@0+(@1)+1
	ORI  R30,HIGH(@2)
	STS  @0+(@1)+1,R30
	.ENDM

	.MACRO __ORD1N
	ORI  R30,LOW(@0)
	ORI  R31,HIGH(@0)
	ORI  R22,BYTE3(@0)
	ORI  R23,BYTE4(@0)
	.ENDM

	.MACRO __ORD2N
	ORI  R26,LOW(@0)
	ORI  R27,HIGH(@0)
	ORI  R24,BYTE3(@0)
	ORI  R25,BYTE4(@0)
	.ENDM

	.MACRO __DELAY_USB
	LDI  R24,LOW(@0)
__DELAY_USB_LOOP:
	DEC  R24
	BRNE __DELAY_USB_LOOP
	.ENDM

	.MACRO __DELAY_USW
	LDI  R24,LOW(@0)
	LDI  R25,HIGH(@0)
__DELAY_USW_LOOP:
	SUBI R24,1
	SBCI R25,0
	BRNE __DELAY_USW_LOOP
	.ENDM

	.MACRO __POINTB1MN
	LDI  R30,LOW(@0+(@1))
	.ENDM

	.MACRO __POINTW1MN
	LDI  R30,LOW(@0+(@1))
	LDI  R31,HIGH(@0+(@1))
	.ENDM

	.MACRO __POINTD1M
	LDI  R30,LOW(@0)
	LDI  R31,HIGH(@0)
	LDI  R22,BYTE3(@0)
	LDI  R23,BYTE4(@0)
	.ENDM

	.MACRO __POINTW1FN
	LDI  R30,LOW(2*@0+(@1))
	LDI  R31,HIGH(2*@0+(@1))
	.ENDM

	.MACRO __POINTD1FN
	LDI  R30,LOW(2*@0+(@1))
	LDI  R31,HIGH(2*@0+(@1))
	LDI  R22,BYTE3(2*@0+(@1))
	LDI  R23,BYTE4(2*@0+(@1))
	.ENDM

	.MACRO __POINTB2MN
	LDI  R26,LOW(@0+(@1))
	.ENDM

	.MACRO __POINTW2MN
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
	.ENDM

	.MACRO __POINTW2FN
	LDI  R26,LOW(2*@0+(@1))
	LDI  R27,HIGH(2*@0+(@1))
	.ENDM

	.MACRO __POINTD2FN
	LDI  R26,LOW(2*@0+(@1))
	LDI  R27,HIGH(2*@0+(@1))
	LDI  R24,BYTE3(2*@0+(@1))
	LDI  R25,BYTE4(2*@0+(@1))
	.ENDM

	.MACRO __POINTBRM
	LDI  R@0,LOW(@1)
	.ENDM

	.MACRO __POINTWRM
	LDI  R@0,LOW(@2)
	LDI  R@1,HIGH(@2)
	.ENDM

	.MACRO __POINTBRMN
	LDI  R@0,LOW(@1+(@2))
	.ENDM

	.MACRO __POINTWRMN
	LDI  R@0,LOW(@2+(@3))
	LDI  R@1,HIGH(@2+(@3))
	.ENDM

	.MACRO __POINTWRFN
	LDI  R@0,LOW(@2*2+(@3))
	LDI  R@1,HIGH(@2*2+(@3))
	.ENDM

	.MACRO __GETD1N
	LDI  R30,LOW(@0)
	LDI  R31,HIGH(@0)
	LDI  R22,BYTE3(@0)
	LDI  R23,BYTE4(@0)
	.ENDM

	.MACRO __GETD2N
	LDI  R26,LOW(@0)
	LDI  R27,HIGH(@0)
	LDI  R24,BYTE3(@0)
	LDI  R25,BYTE4(@0)
	.ENDM

	.MACRO __GETB1MN
	LDS  R30,@0+(@1)
	.ENDM

	.MACRO __GETB1HMN
	LDS  R31,@0+(@1)
	.ENDM

	.MACRO __GETW1MN
	LDS  R30,@0+(@1)
	LDS  R31,@0+(@1)+1
	.ENDM

	.MACRO __GETD1MN
	LDS  R30,@0+(@1)
	LDS  R31,@0+(@1)+1
	LDS  R22,@0+(@1)+2
	LDS  R23,@0+(@1)+3
	.ENDM

	.MACRO __GETBRMN
	LDS  R@0,@1+(@2)
	.ENDM

	.MACRO __GETWRMN
	LDS  R@0,@2+(@3)
	LDS  R@1,@2+(@3)+1
	.ENDM

	.MACRO __GETWRZ
	LDD  R@0,Z+@2
	LDD  R@1,Z+@2+1
	.ENDM

	.MACRO __GETD2Z
	LDD  R26,Z+@0
	LDD  R27,Z+@0+1
	LDD  R24,Z+@0+2
	LDD  R25,Z+@0+3
	.ENDM

	.MACRO __GETB2MN
	LDS  R26,@0+(@1)
	.ENDM

	.MACRO __GETW2MN
	LDS  R26,@0+(@1)
	LDS  R27,@0+(@1)+1
	.ENDM

	.MACRO __GETD2MN
	LDS  R26,@0+(@1)
	LDS  R27,@0+(@1)+1
	LDS  R24,@0+(@1)+2
	LDS  R25,@0+(@1)+3
	.ENDM

	.MACRO __PUTB1MN
	STS  @0+(@1),R30
	.ENDM

	.MACRO __PUTW1MN
	STS  @0+(@1),R30
	STS  @0+(@1)+1,R31
	.ENDM

	.MACRO __PUTD1MN
	STS  @0+(@1),R30
	STS  @0+(@1)+1,R31
	STS  @0+(@1)+2,R22
	STS  @0+(@1)+3,R23
	.ENDM

	.MACRO __PUTB1EN
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
	RCALL __EEPROMWRB
	.ENDM

	.MACRO __PUTW1EN
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
	RCALL __EEPROMWRW
	.ENDM

	.MACRO __PUTD1EN
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
	RCALL __EEPROMWRD
	.ENDM

	.MACRO __PUTBR0MN
	STS  @0+(@1),R0
	.ENDM

	.MACRO __PUTBMRN
	STS  @0+(@1),R@2
	.ENDM

	.MACRO __PUTWMRN
	STS  @0+(@1),R@2
	STS  @0+(@1)+1,R@3
	.ENDM

	.MACRO __PUTBZR
	STD  Z+@1,R@0
	.ENDM

	.MACRO __PUTWZR
	STD  Z+@2,R@0
	STD  Z+@2+1,R@1
	.ENDM

	.MACRO __GETW1R
	MOV  R30,R@0
	MOV  R31,R@1
	.ENDM

	.MACRO __GETW2R
	MOV  R26,R@0
	MOV  R27,R@1
	.ENDM

	.MACRO __GETWRN
	LDI  R@0,LOW(@2)
	LDI  R@1,HIGH(@2)
	.ENDM

	.MACRO __PUTW1R
	MOV  R@0,R30
	MOV  R@1,R31
	.ENDM

	.MACRO __PUTW2R
	MOV  R@0,R26
	MOV  R@1,R27
	.ENDM

	.MACRO __ADDWRN
	SUBI R@0,LOW(-@2)
	SBCI R@1,HIGH(-@2)
	.ENDM

	.MACRO __ADDWRR
	ADD  R@0,R@2
	ADC  R@1,R@3
	.ENDM

	.MACRO __SUBWRN
	SUBI R@0,LOW(@2)
	SBCI R@1,HIGH(@2)
	.ENDM

	.MACRO __SUBWRR
	SUB  R@0,R@2
	SBC  R@1,R@3
	.ENDM

	.MACRO __ANDWRN
	ANDI R@0,LOW(@2)
	ANDI R@1,HIGH(@2)
	.ENDM

	.MACRO __ANDWRR
	AND  R@0,R@2
	AND  R@1,R@3
	.ENDM

	.MACRO __ORWRN
	ORI  R@0,LOW(@2)
	ORI  R@1,HIGH(@2)
	.ENDM

	.MACRO __ORWRR
	OR   R@0,R@2
	OR   R@1,R@3
	.ENDM

	.MACRO __EORWRR
	EOR  R@0,R@2
	EOR  R@1,R@3
	.ENDM

	.MACRO __GETBRS
	SUBI R28,-@1
	LD   R@0,Y
	.ENDM

	.MACRO __PUTBSR
	SUBI R28,-@1
	ST   Y,R@0
	.ENDM

	.MACRO __GETWRS
	SUBI R28,-@2
	LD   R@0,Y+
	LD   R@1,Y
	.ENDM

	.MACRO __PUTWSR
	SUBI R28,-@2
	ST   Y+,R@0
	ST   Y,R@1
	.ENDM

	.MACRO __MOVEWRR
	MOV  R@0,R@2
	MOV  R@1,R@3
	.ENDM

	.MACRO __INWR
	IN   R@0,@2
	IN   R@1,@2+1
	.ENDM

	.MACRO __OUTWR
	OUT  @2+1,R@1
	OUT  @2,R@0
	.ENDM

	.MACRO __CALL1MN
	LDS  R30,@0+(@1)
	LDS  R31,@0+(@1)+1
	ICALL
	.ENDM

	.MACRO __CALL1FN
	LDI  R30,LOW(2*@0+(@1))
	LDI  R31,HIGH(2*@0+(@1))
	RCALL __GETW1PF
	ICALL
	.ENDM

	.MACRO __CALL2EN
	PUSH R26
	PUSH R27
	LDI  R26,LOW(@0+(@1))
	LDI  R27,HIGH(@0+(@1))
	RCALL __EEPROMRDW
	POP  R27
	POP  R26
	ICALL
	.ENDM

	.MACRO __CALL2EX
	SUBI R26,LOW(-@0)
	SBCI R27,HIGH(-@0)
	RCALL __EEPROMRDD
	ICALL
	.ENDM

	.MACRO __GETW1STACK
	IN   R30,SPL
	SUBI R30,LOW(-@0-1)
	LDI  R31,0
	PUSH R16
	LD   R16,Z+
	LD   R31,Z
	MOV  R30,R16
	POP  R16
	.ENDM

	.MACRO __GETD1STACK
	IN   R30,SPL
	IN   R31,SPH
	ADIW R30,@0+1
	LD   R0,Z+
	LD   R1,Z+
	LD   R22,Z
	MOV  R30,R0
	MOV  R31,R1
	.ENDM

	.MACRO __NBST
	BST  R@0,@1
	IN   R30,SREG
	LDI  R31,0x40
	EOR  R30,R31
	OUT  SREG,R30
	.ENDM


	.MACRO __PUTB1SN
	LD   R26,Y
	SUBI R26,-@0
	LDI  R27,0
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SN
	LD   R26,Y
	SUBI R26,-@0
	LDI  R27,0
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SN
	LD   R26,Y
	SUBI R26,-@0
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1SNS
	LD   R26,Y
	SUBI R26,-@0
	LDI  R27,0
	ST   X,R30
	.ENDM

	.MACRO __PUTW1SNS
	LD   R26,Y
	SUBI R26,-@0
	LDI  R27,0
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1SNS
	LD   R26,Y
	SUBI R26,-@0
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RN
	MOV  R26,R@0
	SUBI R26,-@1
	LDI  R27,0
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RN
	MOV  R26,R@0
	SUBI R26,-@1
	LDI  R27,0
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RN
	MOV  R26,R@0
	SUBI R26,-@1
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1RNS
	MOV  R26,R@0
	SUBI R26,-@1
	LDI  R27,0
	ST   X,R30
	.ENDM

	.MACRO __PUTW1RNS
	MOV  R26,R@0
	SUBI R26,-@1
	LDI  R27,0
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1RNS
	MOV  R26,R@0
	SUBI R26,-@1
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1PMN
	LDS  R26,@0
	SUBI R26,-@1
	LDI  R27,0
	ST   X,R30
	.ENDM

	.MACRO __PUTW1PMN
	LDS  R26,@0
	SUBI R26,-@1
	LDI  R27,0
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1PMN
	LDS  R26,@0
	SUBI R26,-@1
	RCALL __PUTDP1
	.ENDM

	.MACRO __PUTB1PMNS
	LDS  R26,@0
	SUBI R26,-@1
	LDI  R27,0
	ST   X,R30
	.ENDM

	.MACRO __PUTW1PMNS
	LDS  R26,@0
	SUBI R26,-@1
	LDI  R27,0
	ST   X+,R30
	ST   X,R31
	.ENDM

	.MACRO __PUTD1PMNS
	LDS  R26,@0
	SUBI R26,-@1
	RCALL __PUTDP1
	.ENDM

	.CSEG
	.ORG 0x00

;START OF CODE MARKER
__START_OF_CODE:

;INTERRUPT VECTORS
	RJMP __RESET
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00
	RJMP 0x00

_0x2000060:
	.DB  0x1
_0x2000000:
	.DB  0x2D,0x4E,0x41,0x4E,0x0,0x49,0x4E,0x46
	.DB  0x0

__GLOBAL_INI_TBL:
	.DW  0x01
	.DW  __seed_G100
	.DW  _0x2000060*2

_0xFFFFFFFF:
	.DW  0

#define __GLOBAL_INI_TBL_PRESENT 1

__RESET:
	CLI
	CLR  R30

;CLEAR SRAM
	LDI  R24,__CLEAR_SRAM_SIZE
	LDI  R26,LOW(__SRAM_START)
	LDI  R27,HIGH(__SRAM_START)
__CLEAR_SRAM:
	ST   X+,R30
	DEC  R24
	BRNE __CLEAR_SRAM

;GLOBAL VARIABLES INITIALIZATION
	LDI  R30,LOW(__GLOBAL_INI_TBL*2+0x4000)
	LDI  R31,HIGH(__GLOBAL_INI_TBL*2+0x4000)
__GLOBAL_INI_NEXT:
	LD   R24,Z+
	LD   R25,Z+
	TST  R24
	BREQ __GLOBAL_INI_END
	LD   R26,Z+
	LD   R27,Z+
	LD   R28,Z+
	LD   R29,Z+
	SUBI R29,-0x40
__GLOBAL_INI_LOOP:
	LD   R16,Y+
	ST   X+,R16
	DEC  R24
	BRNE __GLOBAL_INI_LOOP
	RJMP __GLOBAL_INI_NEXT
__GLOBAL_INI_END:

;HARDWARE STACK POINTER INITIALIZATION
	LDI  R30,LOW(__SRAM_END-__HEAP_SIZE)
	OUT  SPL,R30
	LDI  R30,HIGH(__SRAM_END-__HEAP_SIZE)
	OUT  SPH,R30

;DATA STACK POINTER INITIALIZATION
	LDI  R28,LOW(__SRAM_START+__DSTACK_SIZE)
	LDI  R29,HIGH(__SRAM_START+__DSTACK_SIZE)

	RJMP _main

	.ESEG
	.ORG 0

	.DSEG
	.ORG 0x48

	.CSEG
;/*
; * LCDS.c
; *
; * Created: 20.01.2016 16:03:18
; * Author: Evgeny
; */
;
;#include <io.h>
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x01
	.EQU __sm_mask=0x0E
	.EQU __sm_adc_noise_red=0x02
	.EQU __sm_powerdown=0x04
  .EQU __sm_standby=0x08
	.SET power_ctrl_reg=smcr
	#endif
;#include <delay.h>
;#include <stdlib.h>
;#define F_CPU 8000000UL
;
;
;volatile unsigned char value = 0;
;int i;
;int numFlash = 0;
;int interval = 0;
;volatile unsigned int curPotValue;
;
;void MeasureBattery()
; 0000 0015 {

	.CSEG
_MeasureBattery:
; .FSTART _MeasureBattery
; 0000 0016     ADMUX = 1;
	LDI  R30,LOW(1)
	OUT  0x1B,R30
; 0000 0017     delay_ms(1);
	LDI  R26,LOW(1)
	LDI  R27,0
	RCALL _delay_ms
; 0000 0018 
; 0000 0019     ADCSRA |= (1<<ADSC);
	SBI  0x1D,6
; 0000 001A     while (ADCSRA & (1<<ADSC))
_0x3:
	SBIS 0x1D,6
	RJMP _0x5
; 0000 001B 
; 0000 001C     value = ADCL;
	IN   R30,0x19
	STS  _value,R30
	RJMP _0x3
_0x5:
; 0000 001D }
	RET
; .FEND
;
;unsigned char MeasurePot()
; 0000 0020 {
_MeasurePot:
; .FSTART _MeasurePot
; 0000 0021     ADMUX = 2;
	LDI  R30,LOW(2)
	OUT  0x1B,R30
; 0000 0022     delay_ms(1);
	LDI  R26,LOW(1)
	LDI  R27,0
	RCALL _delay_ms
; 0000 0023 
; 0000 0024     ADCSRA |= (1<<ADSC);
	SBI  0x1D,6
; 0000 0025     while (ADCSRA & (1<<ADSC))
_0x6:
	SBIS 0x1D,6
	RJMP _0x8
; 0000 0026 
; 0000 0027     return ADCL;
	IN   R30,0x19
	RET
; 0000 0028 }
	RJMP _0x6
_0x8:
	RET
; .FEND
;
;void main( void )
; 0000 002B {
_main:
; .FSTART _main
; 0000 002C         // Main Clock source: Calibrated Internal 8 MHz Osc.
; 0000 002D     CCP=0xd8;
	LDI  R30,LOW(216)
	OUT  0x3C,R30
; 0000 002E     CLKMSR=(0<<CLKMS1) | (0<<CLKMS0);
	LDI  R30,LOW(0)
	OUT  0x37,R30
; 0000 002F     // Clock Prescaler division factor: 1
; 0000 0030     CCP=0xd8;
	LDI  R30,LOW(216)
	OUT  0x3C,R30
; 0000 0031     CLKPSR=(0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);
	LDI  R30,LOW(0)
	OUT  0x36,R30
; 0000 0032     // internal_clk = 8 MHz
; 0000 0033 
; 0000 0034 
; 0000 0035     // pulse period 20mS, lenght 1 - 1.5(middle) - 2mS
; 0000 0036     // 8 /64 timer counter prescaler /187 timer value = 668.44Hz (1.49mS)
; 0000 0037 
; 0000 0038     //WGM3:0 = 0101 -> Fast PWM 8 bit
; 0000 0039     TCCR0A = (1<<COM0A1) | (0<<COM0A0) | (0<<WGM01) | (1<<WGM00);
	LDI  R30,LOW(129)
	OUT  0x2E,R30
; 0000 003A     TCCR0B = (0<<WGM03) | (1<<WGM02) | (1<<CS01) | (0<<CS00);
	LDI  R30,LOW(10)
	OUT  0x2D,R30
; 0000 003B 
; 0000 003C      // Enable ADC, presc 1:8 for 125kHz ADC-clock
; 0000 003D     ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);
	LDI  R30,LOW(131)
	OUT  0x1D,R30
; 0000 003E     ADCSRB = 0;
	LDI  R30,LOW(0)
	OUT  0x1C,R30
; 0000 003F     DIDR0 = (1<<ADC2D) | (1<<ADC1D);
	LDI  R30,LOW(6)
	OUT  0x17,R30
; 0000 0040 
; 0000 0041 
; 0000 0042     DDRB = 1<<PORTB0;           //timer out
	LDI  R30,LOW(1)
	OUT  0x1,R30
; 0000 0043     //PUEB = 1<<PUEB1;
; 0000 0044 
; 0000 0045     //delay_ms(200);
; 0000 0046 
; 0000 0047     for(i=0; i<40; i++) MeasureBattery();
	LDI  R30,LOW(0)
	STS  _i,R30
	STS  _i+1,R30
_0xA:
	LDS  R26,_i
	LDS  R27,_i+1
	CPI  R26,LOW(0x28)
	LDI  R30,HIGH(0x28)
	CPC  R27,R30
	BRLT PC+2
	RJMP _0xB
	RCALL _MeasureBattery
_0x9:
	LDS  R30,_i
	LDS  R31,_i+1
	SUBI R30,LOW(-1)
	SBCI R31,0xFF
	STS  _i,R30
	STS  _i+1,R31
	RJMP _0xA
_0xB:
; 0000 004A if(value<= 208)                  numFlash = 1;
	LDS  R26,_value
	CPI  R26,LOW(0xD1)
	BRLO PC+2
	RJMP _0xC
	LDI  R30,LOW(1)
	LDI  R31,HIGH(1)
	STS  _numFlash,R30
	STS  _numFlash+1,R31
; 0000 004B     else if(value>208 && value<=214) numFlash = 2; //3.55-3.65V
	RJMP _0xD
_0xC:
	LDS  R26,_value
	CPI  R26,LOW(0xD1)
	BRSH PC+2
	RJMP _0xF
	LDS  R26,_value
	CPI  R26,LOW(0xD7)
	BRLO PC+2
	RJMP _0xF
	RJMP _0x10
_0xF:
	RJMP _0xE
_0x10:
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	STS  _numFlash,R30
	STS  _numFlash+1,R31
; 0000 004C     else if(value>214 && value<=220) numFlash = 3; //3.65-3.75V
	RJMP _0x11
_0xE:
	LDS  R26,_value
	CPI  R26,LOW(0xD7)
	BRSH PC+2
	RJMP _0x13
	LDS  R26,_value
	CPI  R26,LOW(0xDD)
	BRLO PC+2
	RJMP _0x13
	RJMP _0x14
_0x13:
	RJMP _0x12
_0x14:
	LDI  R30,LOW(3)
	LDI  R31,HIGH(3)
	STS  _numFlash,R30
	STS  _numFlash+1,R31
; 0000 004D     else if(value>220 && value<=226) numFlash = 4; //3.75-3.85V
	RJMP _0x15
_0x12:
	LDS  R26,_value
	CPI  R26,LOW(0xDD)
	BRSH PC+2
	RJMP _0x17
	LDS  R26,_value
	CPI  R26,LOW(0xE3)
	BRLO PC+2
	RJMP _0x17
	RJMP _0x18
_0x17:
	RJMP _0x16
_0x18:
	LDI  R30,LOW(4)
	LDI  R31,HIGH(4)
	STS  _numFlash,R30
	STS  _numFlash+1,R31
; 0000 004E     else if(value>226 && value<=232) numFlash = 5; //3.85-3.95V
	RJMP _0x19
_0x16:
	LDS  R26,_value
	CPI  R26,LOW(0xE3)
	BRSH PC+2
	RJMP _0x1B
	LDS  R26,_value
	CPI  R26,LOW(0xE9)
	BRLO PC+2
	RJMP _0x1B
	RJMP _0x1C
_0x1B:
	RJMP _0x1A
_0x1C:
	LDI  R30,LOW(5)
	LDI  R31,HIGH(5)
	STS  _numFlash,R30
	STS  _numFlash+1,R31
; 0000 004F     else if(value>232 && value<=238) numFlash = 6; //3.95-4.05V
	RJMP _0x1D
_0x1A:
	LDS  R26,_value
	CPI  R26,LOW(0xE9)
	BRSH PC+2
	RJMP _0x1F
	LDS  R26,_value
	CPI  R26,LOW(0xEF)
	BRLO PC+2
	RJMP _0x1F
	RJMP _0x20
_0x1F:
	RJMP _0x1E
_0x20:
	LDI  R30,LOW(6)
	LDI  R31,HIGH(6)
	STS  _numFlash,R30
	STS  _numFlash+1,R31
; 0000 0050     else if(value>238 && value<=244) numFlash = 7; //4.05-4.15V
	RJMP _0x21
_0x1E:
	LDS  R26,_value
	CPI  R26,LOW(0xEF)
	BRSH PC+2
	RJMP _0x23
	LDS  R26,_value
	CPI  R26,LOW(0xF5)
	BRLO PC+2
	RJMP _0x23
	RJMP _0x24
_0x23:
	RJMP _0x22
_0x24:
	LDI  R30,LOW(7)
	LDI  R31,HIGH(7)
	STS  _numFlash,R30
	STS  _numFlash+1,R31
; 0000 0051     else if(value>244)               numFlash = 8; //>4.15V
	RJMP _0x25
_0x22:
	LDS  R26,_value
	CPI  R26,LOW(0xF5)
	BRSH PC+2
	RJMP _0x26
	LDI  R30,LOW(8)
	LDI  R31,HIGH(8)
	STS  _numFlash,R30
	STS  _numFlash+1,R31
; 0000 0052 
; 0000 0053 
; 0000 0054     for(i=0; i<numFlash; i++)
_0x26:
_0x25:
_0x21:
_0x1D:
_0x19:
_0x15:
_0x11:
_0xD:
	LDI  R30,LOW(0)
	STS  _i,R30
	STS  _i+1,R30
_0x28:
	LDS  R30,_numFlash
	LDS  R31,_numFlash+1
	LDS  R26,_i
	LDS  R27,_i+1
	CP   R26,R30
	CPC  R27,R31
	BRLT PC+2
	RJMP _0x29
; 0000 0055     {
; 0000 0056         OCR0A = 30;
	LDI  R30,LOW(30)
	LDI  R31,HIGH(30)
	OUT  0x26+1,R31
	OUT  0x26,R30
; 0000 0057         delay_ms(330);
	LDI  R26,LOW(330)
	LDI  R27,HIGH(330)
	RCALL _delay_ms
; 0000 0058         OCR0A = 2;
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	OUT  0x26+1,R31
	OUT  0x26,R30
; 0000 0059         delay_ms(330);
	LDI  R26,LOW(330)
	LDI  R27,HIGH(330)
	RCALL _delay_ms
; 0000 005A     }
_0x27:
	LDS  R30,_i
	LDS  R31,_i+1
	SUBI R30,LOW(-1)
	SBCI R31,0xFF
	STS  _i,R30
	STS  _i+1,R31
	RJMP _0x28
_0x29:
; 0000 005B 
; 0000 005C     for(;;)
_0x2B:
; 0000 005D     {
; 0000 005E         delay_ms(50);
	LDI  R26,LOW(50)
	LDI  R27,0
	RCALL _delay_ms
; 0000 005F         for(i=0; i<40; i++) MeasurePot();
	LDI  R30,LOW(0)
	STS  _i,R30
	STS  _i+1,R30
_0x2E:
	LDS  R26,_i
	LDS  R27,_i+1
	CPI  R26,LOW(0x28)
	LDI  R30,HIGH(0x28)
	CPC  R27,R30
	BRLT PC+2
	RJMP _0x2F
	RCALL _MeasurePot
_0x2D:
	LDS  R30,_i
	LDS  R31,_i+1
	SUBI R30,LOW(-1)
	SBCI R31,0xFF
	STS  _i,R30
	STS  _i+1,R31
	RJMP _0x2E
_0x2F:
; 0000 0060 curPotValue = 0;
	LDI  R30,LOW(0)
	STS  _curPotValue,R30
	STS  _curPotValue+1,R30
; 0000 0061         for(i=0; i<16; i++) curPotValue += MeasurePot();
	STS  _i,R30
	STS  _i+1,R30
_0x31:
	LDS  R26,_i
	LDS  R27,_i+1
	CPI  R26,LOW(0x10)
	LDI  R30,HIGH(0x10)
	CPC  R27,R30
	BRLT PC+2
	RJMP _0x32
	RCALL _MeasurePot
	LDI  R31,0
	LDS  R26,_curPotValue
	LDS  R27,_curPotValue+1
	ADD  R30,R26
	ADC  R31,R27
	STS  _curPotValue,R30
	STS  _curPotValue+1,R31
_0x30:
	LDS  R30,_i
	LDS  R31,_i+1
	SUBI R30,LOW(-1)
	SBCI R31,0xFF
	STS  _i,R30
	STS  _i+1,R31
	RJMP _0x31
_0x32:
; 0000 0062 OCR0A = curPotValue>>4;
	LDS  R30,_curPotValue
	LDS  R31,_curPotValue+1
	RCALL __LSRW4
	OUT  0x26+1,R31
	OUT  0x26,R30
; 0000 0063 
; 0000 0064         if(++interval == (15*20))
	LDS  R30,_interval
	LDS  R31,_interval+1
	SUBI R30,LOW(-1)
	SBCI R31,0xFF
	STS  _interval,R30
	STS  _interval+1,R31
	CPI  R30,LOW(0x12C)
	LDI  R26,HIGH(0x12C)
	CPC  R31,R26
	BREQ PC+2
	RJMP _0x33
; 0000 0065         {
; 0000 0066             delay_ms(10);
	LDI  R26,LOW(10)
	LDI  R27,0
	RCALL _delay_ms
; 0000 0067             for(i=0; i<80; i++) MeasureBattery();
	LDI  R30,LOW(0)
	STS  _i,R30
	STS  _i+1,R30
_0x35:
	LDS  R26,_i
	LDS  R27,_i+1
	CPI  R26,LOW(0x50)
	LDI  R30,HIGH(0x50)
	CPC  R27,R30
	BRLT PC+2
	RJMP _0x36
	RCALL _MeasureBattery
_0x34:
	LDS  R30,_i
	LDS  R31,_i+1
	SUBI R30,LOW(-1)
	SBCI R31,0xFF
	STS  _i,R30
	STS  _i+1,R31
	RJMP _0x35
_0x36:
; 0000 0069 if(value < 200 && value > 80)
	LDS  R26,_value
	CPI  R26,LOW(0xC8)
	BRLO PC+2
	RJMP _0x38
	LDS  R26,_value
	CPI  R26,LOW(0x51)
	BRSH PC+2
	RJMP _0x38
	RJMP _0x39
_0x38:
	RJMP _0x37
_0x39:
; 0000 006A             {
; 0000 006B                 for(i=0; i<3; i++)
	LDI  R30,LOW(0)
	STS  _i,R30
	STS  _i+1,R30
_0x3B:
	LDS  R26,_i
	LDS  R27,_i+1
	CPI  R26,LOW(0x3)
	LDI  R30,HIGH(0x3)
	CPC  R27,R30
	BRLT PC+2
	RJMP _0x3C
; 0000 006C                 {
; 0000 006D                     OCR0A = curPotValue>10?curPotValue:20;
	LDS  R26,_curPotValue
	LDS  R27,_curPotValue+1
	CPI  R26,LOW(0xB)
	LDI  R30,HIGH(0xB)
	CPC  R27,R30
	BRSH PC+2
	RJMP _0x3D
	LDS  R30,_curPotValue
	LDS  R31,_curPotValue+1
	RJMP _0x3E
_0x3D:
	LDI  R30,LOW(20)
	LDI  R31,HIGH(20)
_0x3E:
_0x3F:
	OUT  0x26+1,R31
	OUT  0x26,R30
; 0000 006E                     delay_ms(330);
	LDI  R26,LOW(330)
	LDI  R27,HIGH(330)
	RCALL _delay_ms
; 0000 006F                     OCR0A = 2;
	LDI  R30,LOW(2)
	LDI  R31,HIGH(2)
	OUT  0x26+1,R31
	OUT  0x26,R30
; 0000 0070                     delay_ms(330);
	LDI  R26,LOW(330)
	LDI  R27,HIGH(330)
	RCALL _delay_ms
; 0000 0071                 }
_0x3A:
	LDS  R30,_i
	LDS  R31,_i+1
	SUBI R30,LOW(-1)
	SBCI R31,0xFF
	STS  _i,R30
	STS  _i+1,R31
	RJMP _0x3B
_0x3C:
; 0000 0072             }
; 0000 0073 
; 0000 0074             //OCR0A = value;
; 0000 0075             //delay_ms(1000);
; 0000 0076             interval = 0;
_0x37:
	LDI  R30,LOW(0)
	STS  _interval,R30
	STS  _interval+1,R30
; 0000 0077         }
; 0000 0078     }
_0x33:
_0x2A:
	RJMP _0x2B
_0x2C:
; 0000 0079 }
_0x40:
	RJMP _0x40
; .FEND
;
;
;//
;//// Timer 0 overflow interrupt service routine
;//interrupt [TIM0_OVF] void timer0_ovf_isr()
;//{
;//    static unsigned char int_count = 0;     //to make the servo signal
;//    // 1mS =180, 1.5mS=143, 2mS=105, 2.1=100
;//
;//    if(int_count++ >= 5)
;//    {
;//        int_count = 0;
;//        if(timer !=0)
;//        {
;//            magic = 130;
;//        }
;//        else
;//        {
;//            if(makePress == 1)
;//                magic = 10;
;//            else
;//                magic = 130;
;//        }
;//        OCR0A = magic;
;//    }
;//    else
;//    {
;//        OCR0A = 255;            //low level all period (inverse compare out)
;//    }
;//
;//    if(timer) timer--;
;//}

	.CSEG

	.DSEG

	.CSEG

	.CSEG

	.CSEG

	.CSEG

	.DSEG
_value:
	.BYTE 0x1
_i:
	.BYTE 0x2
_numFlash:
	.BYTE 0x2
_interval:
	.BYTE 0x2
_curPotValue:
	.BYTE 0x2
__seed_G100:
	.BYTE 0x4

	.CSEG

	.CSEG
_delay_ms:
	subi r26,0
	sbci r27,0
	breq __delay_ms1
__delay_ms0:
	__DELAY_USW 0x7D0
	wdr
	subi r26,1
	sbci r27,0
	brne __delay_ms0
__delay_ms1:
	ret

__LSRW4:
	LSR  R31
	ROR  R30
__LSRW3:
	LSR  R31
	ROR  R30
__LSRW2:
	LSR  R31
	ROR  R30
	LSR  R31
	ROR  R30
	RET

;END OF CODE MARKER
__END_OF_CODE:
