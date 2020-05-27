#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_i2c.h"
#include "misc.h"

#include "pcd8544.h"
#include "IICServo.h"
#include "main.h"

const unsigned char str1[]="NOKIA 5110";
const unsigned char str2[]="СТАРТОВАЛ";
const unsigned char str3[]="РУССКИЕ БУКВЫ!";
const unsigned char str4[]="YES!";

#define SERVO1 0x04<<1
#define SERVO2 0x06<<1
#define SERVO3 0x08<<1
#define SERVO4 0x0A<<1
#define SERVO5 0x0C<<1

//------------------------------------------------------------------------------
volatile unsigned int delay_val;        // счетчик для delay_ms()
volatile unsigned int counter1000;      // счетчик интервалов (такт 10 мкс)
volatile uint16_t systick_ms = 0, flag2=1;
volatile uint16_t capture[10] = {0}, captureResult[10] = {0}, period;
volatile uint8_t capture_is_first = 1, capture_is_ready = 0;
volatile uint8_t end = 0;
uint16_t SysTickDelay;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Delay( unsigned int delay)
//delay function in tick's SysTickTimer
{
   SysTickDelay = delay;
   while (SysTickDelay != 0);
}


void SysTick_Handler(void)
{
   if (SysTickDelay != 0)
   {
      SysTickDelay--;
   }
//*******************
   if (delay_val > 0) delay_val--;
   	counter1000++;
   	systick_ms++;
}



uint16_t uint16_time_diff(uint16_t now, uint16_t before)
/* Вычисляет разность во времени с учётом переполнения счётчика таймера */
{
  return (now >= before) ? (now - before) : ( now + (uint16_t)(60000 - before));
}


// реализация задержки на delay_value милисекунд
void delay_ms(unsigned int delay_value) {
	delay_val=delay_value*10;
	while (delay_val!=0);
}

// реализация задержки на delay_value в * 10 мкс
void delay_10us(unsigned int delay_value) {
	delay_val=delay_value;
	while (delay_val!=0);
}

void periphInit(void)
{
		SysTick_Config(8000000/8000);

		GPIO_InitTypeDef gpio_cfg;
		GPIO_StructInit(&gpio_cfg);

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

		//out port
		// Вывод
		gpio_cfg.GPIO_Mode = GPIO_Mode_OUT;
		gpio_cfg.GPIO_OType = GPIO_OType_PP;
		gpio_cfg.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_13 | GPIO_Pin_11;
		GPIO_Init(GPIOD, &gpio_cfg);

		//AF settings
		/* Таймер TIM3, канал 1 */
		GPIO_InitTypeDef gpioStruct;
		gpioStruct.GPIO_Mode = GPIO_Mode_AF;
		gpioStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		gpioStruct.GPIO_Speed = GPIO_Speed_50MHz;
		gpioStruct.GPIO_Pin = GPIO_Pin_6;
		GPIO_Init(GPIOA, &gpioStruct);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);

		//timer3 settings
		TIM_TimeBaseInitTypeDef timer_base;
		TIM_TimeBaseStructInit(&timer_base);
		timer_base.TIM_Prescaler = 160;
		timer_base.TIM_Period = 60000;
		timer_base.TIM_ClockDivision = 0;
		timer_base.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM3, &timer_base);

		/* Настраиваем захват сигнала:
		- канал: 1
		- счёт: по нарастанию
		- источник: напрямую со входа
		- делитель: отключен
		- фильтр: отключен */
		TIM_ICInitTypeDef timer_ic;
		timer_ic.TIM_Channel = TIM_Channel_1;
		timer_ic.TIM_ICPolarity = TIM_ICPolarity_Falling;
		timer_ic.TIM_ICSelection = TIM_ICSelection_DirectTI;
		timer_ic.TIM_ICPrescaler = TIM_ICPSC_DIV1;
		timer_ic.TIM_ICFilter = 0;
		TIM_ICInit(TIM3, &timer_ic);

		/* Разрешаем таймеру генерировать прерывание по захвату */
		TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
		/* Включаем таймер */
		TIM_Cmd(TIM3, ENABLE);
		/* Разрешаем прерывания таймера TIM3 */
		NVIC_InitTypeDef NVIC_InitStructure;

		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		NVIC_EnableIRQ(TIM3_IRQn);

		//SPI Init
		gpioStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // we are going to use PB6 and PB9
		gpioStruct.GPIO_Mode = GPIO_Mode_AF;	// set pins to alternate function
		gpioStruct.GPIO_Speed = GPIO_Speed_50MHz;	// set GPIO speed
		gpioStruct.GPIO_OType = GPIO_OType_OD;	// set output to open drain --> the line has to be only pulled low, not driven high
		gpioStruct.GPIO_PuPd = GPIO_PuPd_UP;	// enable pull up resistors
		GPIO_Init(GPIOB, &gpioStruct);	// init GPIOB

		// Connect I2C1 pins to AF
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);	// SCL
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1); // SDA

		I2C_InitTypeDef I2C_InitStruct;
		I2C_DeInit(I2C1);
		I2C_InitStruct.I2C_ClockSpeed = 100000; // 100kHz
		I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;	// I2C mode
		I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;	// 50% duty cycle --> standard
		I2C_InitStruct.I2C_OwnAddress1 = 0x70;	// own address, not relevant in master mode
		I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;	// enable acknowledge when reading (can be changed later on)
		I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // set address length to 7 bit addresses
		I2C_Init(I2C1, &I2C_InitStruct);	// init I2C1
		// enable I2C1
		I2C_Cmd(I2C1, ENABLE);
}


//###################################################################################
//###################################################################################
//###################################################################################
int main(void)
{
	SystemInit();
	periphInit();

     // запуск systick (1 мкс)

    lcd8544_init(); // запуск модуля LCD

    I2C1->SR1 &= ~(1<<10);
    unsigned char y=3;
    signed char sy=1;
    while(1)
    {


    	/*ServoCheck(SERVO1);
    	        ServoCheck(SERVO2);
    	        ServoCheck(SERVO3);
    	        ServoCheck(SERVO4);
    	        ServoCheck(SERVO5);
    	        Delay(1);*/
    	ServoPos(SERVO1, 512);
    	ServoPos(SERVO2, 512);
    	ServoPos(SERVO3, 512);
		ServoPos(SERVO4, 512);
		ServoPos(SERVO5, 512);


        lcd8544_clear();
static unsigned char m = 0,k;

/*
		lcd8544_putstr(13,y,str1,1);
		lcd8544_putstr(15,y+10,str2,0);

        lcd8544_putstr(2,y+21,str3,1);
		lcd8544_rect(0,0,78,45,1); //100x63 max
		lcd8544_line(0,0,50,50,1);
		lcd8544_putpix(50,40,1);
*/
		lcd8544_putstr(10,50,str4,k);
		lcd8544_rect(0,0,100,63,1); //100x63 max

        lcd8544_dec(m*100+100,5,2,2,1);

       lcd8544_dec(end,6,2,10,0);
        	lcd8544_dec(flag2,6,2,18,0);
        lcd8544_dec(TIM_GetCapture1(TIM3),6,2,26,0);
        lcd8544_dec(systick_ms,6,2,34,0);

        lcd8544_dec(captureResult[1],6,33,2,0);
        lcd8544_dec(captureResult[2],6,33,10,0);
        lcd8544_dec(captureResult[3],6,33,18,0);
        lcd8544_dec(captureResult[4],6,33,26,0);
        lcd8544_dec(captureResult[5],6,33,34,0);

        lcd8544_dec(captureResult[6],6,65,2,0);
		lcd8544_dec(captureResult[7],6,65,10,0);
		lcd8544_dec(captureResult[8],6,65,18,0);

        //lcd8544_rect(0,0,m/2,m/3,1);
        lcd8544_refresh();
//-------------------------------------------------------------
   //5th servo - 0x0C flag2++


        GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD) ^ GPIO_Pin_13);


/*
        if(k) {ServoPos(SERVO1,450);}
        else {ServoPos(SERVO1,650);}

        if(k) {ServoPos(SERVO2,450);}
        else {ServoPos(SERVO2,650);}

        if(k) {ServoPos(SERVO3,450);}
        else {ServoPos(SERVO3,650);}

        if(k) {ServoPos(SERVO4,450);}
        else {ServoPos(SERVO4,650);}

        if(k) {ServoPos(SERVO5,450);}
        else {ServoPos(SERVO5,650);}
*/


        /*
        //changing adress
        ServoCmd(0x20, 0x84);
        ServoCmd(0x20, 0x88);
        ServoWriteReg8(0x20, 0x20, 0x0C);
        ServoCmd(0x20, 0x86);*/
//****************************************************************
        delay_ms(8000);
        m++;
        if(m%20 > 10) k=0; else k = 1; // flashing string

        y = y + sy;
        if ((y==3) || (y==18)){ sy = -sy; }

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	simple pulse out
        static uint32_t toggle_ms = 0;
		if (uint16_time_diff(systick_ms, toggle_ms) >= 25)
		{
			toggle_ms = systick_ms;
		  GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD) ^ GPIO_Pin_15);
		}
//	end

    }
}
//###################################################################################
//###################################################################################
//###################################################################################




void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_CC1) == SET)
  {
    /* Даём знать, что обработали прерывание */
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

    static uint8_t n = 0;
    static uint16_t last = 0;

    /* Запоминаем предыдущее измерение и считываем текущее */
    uint16_t curr = TIM_GetCapture1(TIM3);
    uint16_t result = uint16_time_diff(curr, last);
    if(curr<last) GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD) ^ GPIO_Pin_13);
    last = curr;

    if(result > 2000)
    {
    	n = 1;
    	GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD) ^ GPIO_Pin_11);
    	end++;
    	if(end > 19)
    	{
    		uint16_t i;
    		for(i=0; i<10; i++) {captureResult[i] = capture[i]/20 ; capture[i]=0;}
    		end = 0;
    					//GPIO_Write(GPIOD, GPIO_ReadOutputData(GPIOD) ^ GPIO_Pin_13);
    	}
    }
    else
    {
    	capture[n++] += result;
    	if(n > 10) {n = 0;flag2++;}
    }

    /* Тут как-нибудь обрабатываем событие over-capture, если провороним */
    if (TIM_GetFlagStatus(TIM3, TIM_FLAG_CC1OF) == SET)
    {
      TIM_ClearFlag(TIM3, TIM_FLAG_CC1OF);
      // ...
      flag2++;
    }
  }
}

