// 20.12.2004 18:22:19 // 28.01.08 // 4.10.08
// mega8
// встроенный RC-генератор 4.0000Mhz 
// ƒмитрий ћосин. startcd@narod.ru  г. ≈лец
//
// 				 http://startcd.narod.ru/ 
//
//*************************************************


/*
74hc595 termo
*/
#include <avr\io.h>
#include <avr\interrupt.h>
#include <util/delay.h> /*F_CPU = 4000000; _delay_ms max = 65,535ms; _delay_us max = 192us */
#include <avr/pgmspace.h>
#include "font.h"

#define	miganie // если обь€вленна	то лини€, раздел€юща€ часы и минуты, мигает.
#define katod//						то вкл. индикаторы с о. катодом.

#ifdef katod
	#warning Common katod
	#else
	#warning Common Anod
#endif

#define BIT _BV
#define F_ 		   (key==12)
#define SET_ 	   (key==8)
#define PLUS 	   (key==0)
#define MINUS	   (key==4)

//*******PORT*******
#define OC1A	BIT(1)
#define DDR_OC1A	DDRB

#define PORTSERIAL	   	PORTB
#define DDRSERIAL	   	DDRB
#define SHIFT		3
#define STORAGE		4
#define DATA		5
#define PIN_SERIAL	   	( (1<<DATA)|(1<<SHIFT)|(1<<STORAGE) )

#define PORT_sig_min	PORTB
#define sig_min_00	BIT(2)

#define	PORTLCD	   PORTD
#define	DDRLCD	   DDRD
#define PINLCD	   PIND

#define	str0	   (PINLCD&BIT(0))
#define str4	   (PINLCD&BIT(1))
#define str8	   (PINLCD&BIT(2))
#define str12	   (PINLCD&BIT(3))
#define	stolb1	   (PINLCD&BIT(4))
#define	stolb2	   (PINLCD&BIT(5))
#define	stolb3	   (PINLCD&BIT(6))
#define	stolb4	   (PINLCD&BIT(7))

//*******PORT*******
#define	PORT_1wire 		PORTC
#define	DDR_1wire 		DDRC
#define	PIN_1wire 		PINC
#define	pwire0 			BIT(3)	//PC3	
#define	pwire1 			BIT(4)	//PC4	

#define PORT_OUT	PORTC
#define DDR_OUT		DDRC
#define PIN_OUT		PINC
#define PIN_calls_osn 	BIT(0)
#define PIN_calls1 		BIT(1)
//#define PIN_timer		BIT(2)
#define PIN_T_STAT		BIT(2)

#define PORT_POWER_DETECT	PORTC
#define PIN_P_Dt			PINC
#define _power_			BIT(5)
#define pin_power_		( PIN_P_Dt & _power_ )



//-------- time_flag byte -------
#define fl_sek_1_switch			0
#define fl_sig_otsr				1
#define	fl_plus					3//знак изменени€ €ркости
#define	fl_sig_butt				4//подача сигнала при нажатии кнопки
#define	fl_power_init_device 	5
#define fl_signal_cls		 	6
#define	fl_signal_out_enable	7


//----------- time_1wire_flag ---
//#define fl_parasite_power		7

//----------timer[3]----------
#define fl_timer_chet		 0

//#define fl_signal_cls		 3
#define fl_calls_pin_A_on	 4
#define fl_calls_pin_B_on	 5
//----------------------------
#define	h_sek		0
#define	h_min		1
#define	h_hour		2
#define	h_day 		3
#define	h_num		4
#define	h_month		5
#define	c_min		0
#define c_hour		1
#define c_day		2
//----------corr_flag---------
 #define fl_corr_znak_pl	0

#define ds0	0
#define ds1	1


#define true	1
#define false	0

//---time[]---
#define wire1		0
#define note		1
#define note_but	2
#define beg_str		3

#define kolvo_bud	16
#define kolvo_osn	8

uint8_t light, p_index; 
const uint8_t light_swith[9]={35,29,27,25,21,18,14,9,5}, light_time_mig[9]={10,9,8,8,7,5,4,4,3};//{10,9,8,7,6,5,4,4,3};
unsigned char key, znakomesto, regim, time_in_regim, anti_drebezg[3];
unsigned char lcd_time_switch, time_flag;
int8_t change_light;
unsigned char  lcd_light=5, day_light=7, night_light=3, tim_c_light, tim_do_light;
unsigned char tempir_lsb[2], tempir_msb[2], temper_termostat;
  signed int  temperatura[2];
unsigned char out_1wire, time_1w_flag, count_com_1w, power_suplly_1w;
unsigned char correction, correct, corr_flag, clock_, isr_1sek_, hour[]={0,0,12,0,0,0};
unsigned char calls[kolvo_bud][3],calls_pin[kolvo_bud-kolvo_osn][3], budilnik_nomer, otsroshka_num, budilnik_play_num, fl_calls_pin;
uint32_t time_bud_pin;
unsigned char count_note;
unsigned char ocr1[5]={4,0,4,0,0};
unsigned char lcd_buffer[10], bcd_num0, bcd_num1, bcd_num2, bcd_num3, bcd_num4;
unsigned char signal_bud, signal, time_signal=59;		
uint8_t vid_t;
#define Nreg_st	16
unsigned char regim_state_time[Nreg_st+1], viewstate, time_view_buf=1; 

  signed char time[4];

#define kolvo_zon	3  
struct {
	uint8_t time[kolvo_zon];
	uint8_t temperature[kolvo_zon];
	uint8_t gisterezis;
	}termostat;
	
unsigned int  mask;

#define	ocr1ahi			0
#define	ocr1alow		1
#define	ocr1ahi_but		2
#define	ocr1alow_but	3

							
// callnomer-5 hour-7,8 min-10,11
//unsigned char *ukaz_na_str;


prog_uint16_t	corr_in_lcd[] =		{0,8,16, 23,31,39,47,55,63,70,78,86,94,102,109,117,
									125,133,141,148,156,164,172,180,188,195,203,211,219,227,234,242,
									250,258,266,273,281,289,297,305,313,320,328,336,344,352,359,367,
									375,383,391};

const unsigned char 	num_in_month[12]={31,28,31,30,31,30,31,31,30,31,30,31};
const unsigned char length_note[4]={20,10,20,75}, uprav_TCCR1B[4]={1|(1<<WGM12), 0|(1<<WGM12), 1|(1<<WGM12), 0|(1<<WGM12)};

//-----------EEPROM-----------------
#define	EEPROM_ocr1a			0x02//1byte
#define	EEPROM_ocr1alow			0x03//1byte
#define	EEPROM_ocr1ahi_but		0x04//1byte
#define	EEPROM_ocr1alow_but		0x05//1byte
#define	EEPROM_verifu_but		0x06//1byte
#define	EEPROM_sig_pin_or_bip	0x07//1byte
#define	EEPROM_correction		0x08//1byte
#define	EEPROM_corr_flag		0x09//1byte
#define EEPROM_otsrochka_num	0x0A//1byte
#define	EEPROM_light			0x0B//2byte
#define	EEPROM_vid_t			0x0F//2byte
#define	EEPROM_regim_state_time	0x10//20byte
#define	EEPROM_termostat		0x40
#define	EEPROM_calls			0x70//byte

