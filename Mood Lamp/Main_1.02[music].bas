$regfile = "m16def.dat"
$crystal = 26000000

Ddra = &B11111000
Ddrb = &B11111111
Ddrc = &B11111111
Ddrd = &B11110000
Porta = &B00000000
Portb = &B11111111
Portc = &B00000000
Portd = &B00000100
'===============================================================================
Dim X As Byte
Dim Y As Byte
Dim Z As Byte
Dim S As Byte
Dim Rgb(15) As Byte
Dim Tik As Byte

R Alias Portd.7
G Alias Portd.6
Btn Alias Pind.2
Zvuk Alias Pina.1
Svet Alias Pina.0
'========================Init===================================================
Config Adc = Single , Prescaler = Auto , Reference = Avcc
Start Adc

Config Timer0 = Timer , Prescale = 8                        'настраиваем таймер
Enable Timer0                                               'разрешаем работу таймера 0
Enable Interrupts                                           'Разрешаем прерывания
On Timer0 Pwm_out                                           'работа по таймеру 0
Start Timer0                                                'старт таймера 0
'===============================================================================

For X = 1 To 15
  Rgb(x) = 255
Next X
'===========================main cycle==========================================
Do

Y = Getadc(1)
Y = Y / 2

For X = 1 To 5
  Z = Lookup(y , R_data)
  Rgb(x) = Lookup(z , Bright )
Next X

For X = 6 To 10
  Z = Lookup(y , G_data)
  Rgb(x) = Lookup(z , Bright )
Next X

For X = 11 To 15
  Z = Lookup(y , B_data)
  Rgb(x) = Lookup(z , Bright )
Next X

Waitus 2000

For X = 1 To 15
Y = Rnd(8)
Rgb(x) = Y
Next X
Waitus 2000

Loop
End
'===============================================================================
Pwm_out:
Incr Tik
 If Tik = 255 Then
  Tik = 0
  Portc = 0
  Porta = 0
  Portd = &B00000100
 End If
   If Tik > Rgb(1) Then Porta.6 = 1                         'R
   If Tik > Rgb(2) Then Porta.7 = 1
   If Tik > Rgb(3) Then Porta.5 = 1
   If Tik > Rgb(4) Then Porta.4 = 1
   If Tik > Rgb(5) Then Portc.7 = 1
   If Tik > Rgb(6) Then Portd.4 = 1                         'G
   If Tik > Rgb(7) Then Portc.3 = 1
   If Tik > Rgb(8) Then Portc.6 = 1
   If Tik > Rgb(9) Then Porta.3 = 1
  If Tik > Rgb(10) Then Portc.0 = 1
  If Tik > Rgb(11) Then Portc.4 = 1                         'B
  If Tik > Rgb(12) Then Portc.5 = 1
  If Tik > Rgb(13) Then Portd.5 = 1
  If Tik > Rgb(14) Then Portc.1 = 1
  If Tik > Rgb(15) Then Portc.2 = 1
Return
'+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
7seg:
Data &B00100001 , &B11110101 , &B00010011 , &B10010001 , &B11000101 , &B10001001 , &B00001001 , &B11110001 , &B00000001 , &B10000001 , &B01000001 , &B00001101 , &B00001011       'B

Bright:
Data 255 , 249 , 243 , 231 , 208 , 162 , 73 , 0
'Data 255 , 253 , 252 , 251 , 249 , 246 , 243 , 238 , 231 , 221 , 208 , 189 , 162 , 125 , 73 , 0

R_data:
Data 7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 6 , 5 , 4 , 3 , 2 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 , 7 , 7 , 7 , 7 , 7 , 7
G_data:
Data 0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 6 , 5 , 4 , 3 , 2 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
B_data:
Data 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 7 , 6 , 5 , 4 , 3 , 2 , 1
'----R-0-------------------------rg-7------------------------G-14------------------------gb-21-----------------------B-28------------------------rb-35-------------------41

'1r -a6
'1g d4
'1b c4
'2r a7
'2g c3
'2b c5
'3r a5
'3g c6
'3b d5
'4r a4
'4g a3
'4b c1
'5r c7
'5g c0
'5b c2