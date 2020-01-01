'�������� 5-��������� ���� �������:
'
'1,2,3 ������ - ������������ ���������� 0...40 �.
'4 ����� (�������������) - ������������ ���������� 0...(-40) �.
'5 ����� - ������������ (����, ����������) � ���������� �������������
'-------------------------------------------------------------------------------
$crystal = 8000000                                          ' 8 MHz   UART
$regfile = "m8def.dat"                                      ' ATMega8
$baud = 115200

Config Lcd = 20 * 4
Config Lcdpin = Pin , Db4 = Portb.6 , Db5 = Portb.7 , Db6 = Portd.5 , Db7 = Portd.6 , E = Portd.7 , Rs = Portb.0
Config Adc = Single , Prescaler = Auto , Reference = Avcc
Config Timer1 = Pwm , Pwm = 10 , Compare A Pwm = Clear Down , Prescale = 1,       ' Compare B Pwm = Clear Down ,
'+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Declare Sub Izmerenie()
Declare Sub Akseler()
Declare Sub Knopka()
Declare Sub Uart()
Declare Sub U_i_set()
Declare Sub Convert()
Declare Sub Displ()
Declare Sub Eeprom()
'===============================================================================
Deflcdchar 1 , 32 , 32 , 4 , 2 , 31 , 2 , 4 , 32            '��������� "�������"
'===============================================================================
Dim M As Byte                                               '����� ����������� ���������:(0-Standby):(1-U):(2-I)

Dim U220_izm As Word                                        '���������� ����

Dim U As Single                                             '��������������� ����������
Dim U_r As Single                                           '�������� ����������
Dim U_izm As Word                                           '���������� � ADC (0-1024)

Dim I As Single                                             '��������������� ���
Dim I_r As Single                                           '�������� ���
Dim I_izm As Word                                           '��� � ADC (0-1024)

Dim Over As String * 4

Dim Pwm1 As Word

Dim Comchar As Word                                         '����������� ������
Dim Buff As String * 5                                      '���������� �����

Dim Str_u1 As String * 4 , Str_i1 As String * 4 , Str_u2 As String * 4 , Str_i2 As String * 4 , Str_u3 As String * 4 , Str_i3 As String * 4 , Str_u4 As String * 4 , Str_i4 As String * 4
'===============================================================================
Const U_res = 1                                             '��������� ���������� � �������� ��������� ����������
Const I_res = 1                                             '��������� ���������� � �������� ��������� ����
Const U_max = 15                                            '������������ ���������� ������������������� ����
Const I_max = 7                                             '������������ ���������� ��� ��������
'===============================================================================
'p_on Alias Portb.3
Ddrb.5 = 1
Led Alias Portb.5                                           '��������� ���������� ����

Ddrd.2 = 0                                                  '=> ������ �� ��������
Portd.2 = 1
But Alias Pind.2
'Powr Alias Portb.5                                          '������ "STOP/Standby"------pull-up

Config Debounce = 1

Ch_a Alias Pind.3
Ch_b Alias Pind.4
Ddrd.3 = 0
Ddrd.4 = 0

Config Int1 = Falling

On Int1 Get_encoder
On Urxc Getchar                                             '�������������� ���������� �� UART
'+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Initialization:
Enable Interrupts
Enable Urxc
Enable Int1

  Start Adc
  M = 1
  Pwm1a = 0

  Cls
  Cursor Off

'------------------------ ��������� --------------------------------------------
Main:
'(If Powr = 0 Then
M = 0                                                       ' 1-�������: ������� ���������
End If
')
Do
 Call Izmerenie                                             '��������� ����, ����������
 Call Akseler                                               '���������� ��������� ��������
 Call Knopka                                                '��������� ������ ��������
 Call U_i_set                                               '��������� ��������� ����������
 Call Convert                                               '��������������� ��������
 Call Displ                                                 '����� ����������� �� �������
 Call Eeprom
Loop
End
'========================������������===========================================
Sub Displ                                                   '==> ���������
 On M Goto Mdi0 , Mdi1 , Mdi2

   Mdi0:                                                    '-> Standby
   Lcd "    ��o� ���a���     "                              '
   Thirdline                                                '
   Lcd "  Ha�pǶe��e ce��"                                  '
   Locate 4 , 5                                             '
   Lcd U220_izm ; " Bo�Ŀ"
   Goto Es

   Mdi1:                                                    '-> ����������
   Locate 1 , 1
   'Lcd Spc(20)
   Lcd Chr(1) ; "U= " ; Fusing(u , "#.#") ; "            "  '
   Locate 2 , 1
   'Lcd Spc(20)
   Lcd " I= " ; Fusing(i_r , "#.###") ; " " ; Over ; "         "
   Goto Uart_display

   Mdi2:                                                    '-> ���
   Locate 1 , 1
   'LCD SPC(20)
   Lcd " U= " ; Fusing(u_r , "#.#") ; "        "            '
   Locate 2 , 1
   'LCD SPC(20)
   Lcd Chr(1) ; "I= " ; Fusing(i_r , "#.###") ; " I{217}= " ; Fusing(i , "#.###") ; "  "
   Gosub Uart_display

   Uart_display:                                            '-> ��������� � UART
   Locate 3 , 1
   Lcd Str_u1 ; "  " ; Str_u2 ; "  " ; Str_u3 ; "  " ; Str_u4
   Fourthline
   Locate 4 , 1
   Lcd Str_i1 ; " " ; Str_i2 ; " " ; Str_i3 ; " " ; Str_i4

   Es:
End Sub
'=============
Sub Izmerenie
On M Goto M_izm0 , M_izm1 , M_izm1

M_izm0:
   U220_izm = Getadc(5)                                     '������ ���
  Goto P1

M_izm1:
   U_izm = Getadc(0)
   I_izm = Getadc(1)

P1:
End Sub
'=============
Sub Convert
   U_r = U_izm * 0.005
   U_r = U_r * U_res

   I_r = I_izm * 0.005
   I_r = I_r * I_res
End Sub
'=============
Sub U_i_set                                                 '==> ��������� ���������� � ����
   On M Goto M_set0 , M_set1 , M_set2

   M_set0:                                                  '-> standby
   Goto P2

   M_set1:                                                  '-> ����� ��������� ����������
   If U > U_r Then                                          '���� �������� ���������� ������ ��������������:
    Pwm1 = Pwm1 + 1                                         '�� ����������� PWM
   Elseif U < U_r Then
    Pwm1 = Pwm1 - 1                                         '����� - ��������� PWM
   End If

   If I_r > I_max Then
    Over = "OVER"
    Led = 1
   Else
    Over = ""
    Led = 0
   End If

   Goto P2

   M_set2:                                                  '-> ����� ��������� ����
   If I_r > I Then
    Pwm1 = Pwm1 - 1                                         '��� ���������� ���� ��������� - ��������� PWM
   Elseif I_r < I Then                                      '���� �������� ��� �� ��������� �������������:
      If U_r < U_max Then
        Pwm1 = Pwm1 + 1                                     '����������� PWM
      Elseif U_r > U_max Then
        Pwm1 = Pwm1 - 1                                     '���������� ���������� - ��������� PWM
      End If
   End If

   Goto P2

   P2:
  If Pwm1 = 4096 Then
   Pwm1 = 4095
  Elseif Pwm1 = 0 Then
   Pwm1 = 1
  End If

End Sub
'=============
'Sub Uart
Getchar:                                                    '==> UART
Disable Interrupts
   Comchar = Inkey()                                        '��������� 1 ������
 Select Case Comchar
    Case 115 : Str_u1 = Buff
     Buff = ""
    Case 116 : Str_i1 = Buff
     Buff = ""
    Case 117 : Str_u2 = Buff
     Buff = ""
    Case 118 : Str_i2 = Buff
     Buff = ""
    Case 119 : Str_u3 = Buff
     Buff = ""
    Case 120 : Str_i3 = Buff
     Buff = ""
    Case 121 : Str_u4 = Buff
     Buff = ""
    Case 122 : Str_i4 = Buff
     Buff = ""
    Case Else : Buff = Buff + Chr(comchar)
 End Select
Enable Interrupts
Return
'End Sub
'=============
Sub Akseler                                                 '==> ���������� ��������

End Sub
'=============
Sub Knopka                                                  '==> ������ ������������ �������
 Debounce But , 0 , Kn_mode                                 '��� �������
End Sub

Kn_mode:
   M = M + 1                                                '����������� �����
   If M = 3 Then
    M = 1
   End If
Return
'=============
Sub Eeprom                                                  '==> EEPROM

End Sub
'=============
Get_encoder:                                                '==> �������
 On M Goto E1 , E2 , E3

  E1:                                                       '1111111
  Goto P3

  E2:                                                       '2222222
  If Ch_b = 0 Then
      U = U - 0.1
  Else
      U = U + 0.1
  End If

  Gifr = 64
  Goto P3

  E3:                                                       '3333333
  If Ch_b = 0 Then
      I = I - 0.001
  Else
      I = I + 0.001
  End If

  Gifr = 64

  P3:
Return
'===============================================================================
'-END---END---END---END---END---END---END---END---END---END---END---END---END---
'===============================================================================