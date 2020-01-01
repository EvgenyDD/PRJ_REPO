'Цифровой 5-канальный блок питания:
'
'1,2,3 каналы - регулируемое напряжение 0...40 В.
'4 канал (отрицательный) - регулируемое напряжение 0...(-40) В.
'5 канал - стабилизатор (тока, напряжения) с програмным обслуживанием
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
Dim M As Byte                                               'режим выполняемой программы:(0-Standby):(1-U):(2-I)

Dim U220_izm As Word                                        'напряжение сети

Dim U As Single                                             'устанавлимаемое напряжение
Dim U_r As Single                                           'реальное напряжение
Dim U_izm As Word                                           'напряжение с ADC (0-1024)

Dim I As Single                                             'устанавливаемый ток
Dim I_r As Single                                           'реальный ток
Dim I_izm As Word                                           'ток с ADC (0-1024)

Dim Over As String * 4

Dim Pwm1 As Word

Dim Comchar As Word                                         'принимаемый символ
Dim Buff As String * 5                                      'переменная буфер

Dim Str_u1 As String * 4 , Str_i1 As String * 4 , Str_u2 As String * 4 , Str_i2 As String * 4 , Str_u3 As String * 4 , Str_i3 As String * 4 , Str_u4 As String * 4 , Str_i4 As String * 4
'===============================================================================
Const U_res = 1                                             'Отношение резисторов в делителе измерения напряжения
Const I_res = 1                                             'Отношение резисторов в делителе измерения тока
Const U_max = 15                                            'Максимальное напряжение пристабилизировании тока
Const I_max = 7                                             'Максимальный допустимый ток нагрузки
'===============================================================================
'p_on Alias Portb.3
Ddrb.5 = 1
Led Alias Portb.5                                           'светодиод превышения тока

Ddrd.2 = 0                                                  '=> кнопка на энкодере
Portd.2 = 1
But Alias Pind.2
'Powr Alias Portb.5                                          'кнопка "STOP/Standby"------pull-up

Config Debounce = 1

Ch_a Alias Pind.3
Ch_b Alias Pind.4
Ddrd.3 = 0
Ddrd.4 = 0

Config Int1 = Falling

On Int1 Get_encoder
On Urxc Getchar                                             'переопределяем прерывание на UART
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

'------------------------ Программа --------------------------------------------
Main:
'(If Powr = 0 Then
M = 0                                                       ' 1-признак: питание выключено
End If
')
Do
 Call Izmerenie                                             'измерение тока, напряжения
 Call Akseler                                               'коэфициент ускорения энкодера
 Call Knopka                                                'обработка кнопки энкодера
 Call U_i_set                                               'установка выходного напряжения
 Call Convert                                               'конвертирование значений
 Call Displ                                                 'вывод изображения на дисплей
 Call Eeprom
Loop
End
'========================Подпрограммы===========================================
Sub Izmerenie
On M Goto M_izm0 , M_izm1 , M_izm1

M_izm0:
   U220_izm = Getadc(5)                                     'нужный пин
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
'Sub Uart
Getchar:                                                    '==> UART
Disable Interrupts
   Comchar = Inkey()                                        'принимаем 1 символ
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
Sub Eeprom                                                  '==> EEPROM

End Sub
'===============================================================================
'-END---END---END---END---END---END---END---END---END---END---END---END---END---
'===============================================================================