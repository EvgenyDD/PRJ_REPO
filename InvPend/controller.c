/* Includes ------------------------------------------------------------------*/
#include "controller.h"
#include <math.h>
#include <stdbool.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
float PGain, DGain, IGain;
float lastPos = 0;
float Error = 0;
float EError = 0;
float curSpeed = 0;

#define MIN_PWM 80
#define iLimit 700

int16_t Center = 1980;

extern int32_t speedOfMove;
extern uint32_t realTrack;
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  :
* Description    :
* Input		 :
*******************************************************************************/

/*******************************************************************************
* Function Name  :
* Description    :
*******************************************************************************/
void InitPWMController()
{
	PGain = 10.0f;
	DGain = 0.010f;
	IGain = 0;

}


/*******************************************************************************
* Function Name  :
* Description    :
*******************************************************************************/
int16_t PWMController(uint16_t center, uint16_t newVal)
{



	static uint8_t counter = 0;

	float real = newVal - center;

	/* if we cross ZERO pos - clear Error4IntegralPart */
	/*static bool flag = false;
	if(real>0 && flag==false)
	{
		flag = true;
		Error = 0;
	}
	else if(real<0 && flag==true)
	{
		flag = false;
		Error = 0;
	}*/

	if(abs(real)>LIMIT) return 0;

	if(counter++ >= 50)
	{
		counter = 0;
		curSpeed = real - lastPos;
		lastPos = real;

		Error += real*0.8f;//IGain;
		if(Error > iLimit) Error = iLimit;
		if(Error < -iLimit) Error = -iLimit;

		EError = (float)((int16_t)realTrack-15000)/100*IGain;
		if(EError > iLimit) EError = iLimit;
		if(EError < -iLimit) EError = -iLimit;
	}

	float out = 0;

	out += real*PGain;

	out += Error;

	out += EError;

	out += curSpeed*5;//DGain;

	if(out>0)
		out += STRT_VAL;
	if(out<0)
		out -= STRT_VAL;

	if(out>(1000)) out = 1000;
	if(out<-1000) out = -1000;

	return (int16_t)out;
}

int16_t VirtZeroController(int16_t realTrack, uint16_t targetPos)
{
#define ZERO 1980
#define GOAL 15000

	static int16_t lastTrack = GOAL;
	int16_t speed = realTrack - lastTrack;
	speedOfMove = speed;

	int16_t zero = ZERO;

	zero -= (int16_t)((float)(realTrack-GOAL)*DGain);
	zero += (int16_t)((float)speed *IGain);
	lastTrack = realTrack;
	return zero;
}
