#include "all.h"
#include "PID.h"


#define PwmA    (OCR1A)
//#define PwmB    (OCR1B)
#define EnA     PORTA |= (1<<4)
#define DisA    PORTA &= ~(1<<4)




#define abs(x) ((x>0)?(x):(-x))



//signed int curSpd;

extern uint8_t failFlag;		//motor is stopped
extern uint16_t sliderValue;	//value from hand potentiometer
extern uint8_t currentSpeed;	//current speed value





signed int DGain = 15;
signed int PGain = 30;

void DriveReset()
{
    PwmA = 0;   
    DisA;
}


void DriveRotate(uint8_t amount)
{
    DisA;

	if(amount <10)
	 	TCCR1A &= ~((1<<COM1A1)|(1<<COM1B1));
	else
		TCCR1A |= ((1<<COM1A1)|(1<<COM1B1));

    PwmA = amount;        
}



void DriveBreak(uint8_t amount)
{
       
}

void SetFail()
{
	failFlag = 1;
	PwmA = 0;	
}


uint8_t DrivePID(int current, int target)
{
/*
	if(failFlag) 
	{
		PwmA = 0;
		return 0;
	}*/



static uint8_t prevSpeed = 0;
static signed int IValue = 0;

	signed int speedDiff = target - current; 		// + if too slow, - if too fast	
/*	signed int accelDiff = current - prevSpeed; 	// + if accelerating, - if deaccelerating
	prevSpeed = current;

	
	IValue += speedDiff;

	if(speedDiff<5) IValue = 0;*/

  	//MAIN PID Processing
   /* signed int PValue = speedDiff;
	signed int DValue = accelDiff;  */
   
#define COMP 10

	signed int out=target*COMP;

//#define IGain    2

   	if(abs(speedDiff) > 2)
	{
		//PGain = 20;
        out += 20*speedDiff;
		PORTA |= (1<<7);
    }
	else
	{
		PORTA &= ~(1<<7);
	}

    //out -= DGain * DValue;

	//out += IGain * IValue;
    out = out /COMP;
    
    if(out > 255)
        out = 255;

	if(out < 0)
		out = 0;

    return (uint8_t)out;
}
