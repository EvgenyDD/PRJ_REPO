$regfile = "m32def.dat"
$crystal = 20000000

Ddra = &B11111111
Ddrb = &B11111111
Ddrc = &B11111111
Ddrd = &B11111111
Porta = &B00001111
Portb = &B00000000
Portc = &B00000000
Portd = &B00000000
'===============================================================================
Dim X As Byte
Dim Y As Byte

Dim Rgb_1 As integer
Dim Rgb_2 As Integer
Dim Rgb_3 As Integer
Dim Rgb_4 As Integer

Dim Tik As Byte

A Alias Porta.0                                             'red
B Alias Porta.2                                             'grn
C Alias Porta.3                                             'blu
D Alias Porta.1                                             'yel
'========================Init===================================================
Config Timer0 = Timer , Prescale = 1                        'настраиваем таймер
Enable Timer0                                               'разрешаем работу таймера 0
Enable Interrupts                                           'Разрешаем прерывания
On Timer0 Pwm_out                                           'работа по таймеру 0
Start Timer0                                                'старт таймера 0
'===============================================================================
'===========================main cycle  ========================================
Do

For X = 1 To 255
Rgb_1 = X
Rgb_2 = X
Rgb_3 = X
Rgb_4 = X
Waitus 800                                                  'мигание
Next X
Waitms 4

For X = 255 To 1 Step -1
Rgb_1 = X
Rgb_2 = X
Rgb_3 = X
Rgb_4 = X
Waitus 800                                                  'мигание
Next X

Waitms 12

Loop
End

'0000000000000000000000000000000000000000000000000000000000000000000000000000000
Pwm_out:
Incr Tik
 If Tik = 255 Then
  Tik = 0
  Porta = &B00001111
 End If
   If Tik > Rgb_1 Then Porta.0 = 0
   If Tik > Rgb_2 Then Porta.1 = 0
   If Tik > Rgb_3 Then Porta.2 = 0
   If Tik > Rgb_4 Then Porta.3 = 0
Return
'+++++++++++++++++++++