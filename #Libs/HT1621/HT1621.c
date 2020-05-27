/*    ���������� ��� ������ � �������� �� ����������� HT1621 
      �� ��������� �������� �����
      
      � ��� - 12 ����, � ����� ����������� ��������
      � ������� ���� ��������� �����. 
*/

#include "HT1621.h"
char digit[10]={ 0x7D, 0x60, 0x3E, 0x7A, 0x63, 0x5B, 0x5F, 0x70, 0x7F, 0x7B }; //������� 0-9

void SendBit_1621(unsigned char data, unsigned char cnt)  //DATAa high cnt low WRite to HT1621,high bit first 
{ 
  unsigned char i; 
  for(i=0; i<cnt; i++){ 
    if((data & 0x80)==0) DATA = 0;
    else DATA = 1; 
    WR = 0; 
    data <<= 1; 
    asm("nop"); 
    WR = 1;     
  } 
}

void SendDataBit_1621(unsigned char data,unsigned char cnt)  //DATAa low cnt bit WRite to the HT1621, low bit first
{ 
  unsigned char i; 
  for(i =0; i < cnt; i++) { 
    if((data&0x01)==0) DATA = 0; 
    else DATA = 1; 
    WR = 0; 
    data >>= 1;
    asm("nop"); 
    WR = 1; 
     
  } 
}

void SendCmd(unsigned char command) 
{ 
  CS = 0; 
  SendBit_1621(0x80,4); //WRite flag code �100� and 9 bit command instruction 
  SendBit_1621(command,8); //no change clock output instruction, for convenience 
  CS = 1; //directly WRite command high bit as �0� 
}

void Write_1621(unsigned char addr,unsigned char data) 
{ 
  CS = 0; 
  addr <<=3;
  addr -= 4;
  SendBit_1621(0xa0,3); //WRite flag code �101� 
  SendBit_1621(addr,6); //WRite addr high 6 bits 
  SendDataBit_1621(data,8);  //WRite DATAa low 4 bits 
  CS = 1; 
} 

void Clear_1621() 
{ 
  CS = 0; 
  SendBit_1621(0xa0,3); //WRite flag code �101� 
  for(int u=0; u<17; u++){
    Write_1621(u,0); 
  }
  CS = 1;
}

void WriteDigit_1621(unsigned char addr,unsigned char data,unsigned char dotPoint) 
{ 
  CS = 0; 
  addr <<=3;
  addr -= 4;
  data = digit[data];
  if(dotPoint) data |= 0x80;
  SendBit_1621(0xa0,3); //WRite flag code �101� 
  SendBit_1621(addr,6); //WRite addr high 6 bits 
  SendDataBit_1621(data,8);  //WRite DATAa low 4 bits 
  CS = 1; 
} 