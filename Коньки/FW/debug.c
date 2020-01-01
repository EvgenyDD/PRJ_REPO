/* Includes ------------------------------------------------------------------*/
#include "debug.h"
#include "string.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
 * Function Name  : DebugInit
 * Description    : Initialize debug (via USART1)
 *******************************************************************************/
void DebugInit()
{
}

/*******************************************************************************
 * Function Name  : DebugSendString
 * Description    : Sends debug information (via USART1)
 * Input          : pointer to text massive
 *******************************************************************************/
void DebugSendString(char *pText)
{
	for (; *pText != '\0'; pText++)
	{
		while (!(UCSRA & (1 << UDRE)))
			; //  Устанавливается, когда регистр свободен
		UDR = *pText;
		//while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
		//USART_SendData(USART1, *pText);
	}
	while (!(UCSRA & (1 << UDRE)))
		; //  Устанавливается, когда регистр свободен
	UDR = '\n';
	//while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
	//USART_SendData(USART1, '\n');

}

/*******************************************************************************
 * Function Name  : DebugSendChar
 * Description    : Sends debug information (via USART1)
 * Input          : char to send
 *******************************************************************************/
void DebugSendChar(char charTx)
{
	while (!(UCSRA & (1 << UDRE)))
		; //  Устанавливается, когда регистр свободен
	UDR = charTx;
	//while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	//USART_SendData(USART1, charTx);
}

void DebugSendNum(int Num)
{
	char str[50];
	itoa_(Num, str);
	DebugSendString(str);
}

void DebugSendNumWDesc(char *string, int Num)
{
	char str[50] =
	{ '\0' }, number[20];
	strcat_(str, string);
	itoa_(Num, number);
	strcat_(str, number);
	DebugSendString(str);
}
