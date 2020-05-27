$regfile = "m32def.dat"
$crystal = 20000000

Ddrd = &B10110000

Config Timer1 = Pwm , Pwm = 8 , Compare A Pwm = Clear Down , Compare B Pwm = Clear Down , Prescale = 1
Config Timer2 = Pwm , Prescale = 1 , Pwm = On , Compare Pwm = Clear Up
Dim M As Byte
Dim X As Byte
Dim Y As Byte
Dim Z As Byte

A Alias Pwm1a
B Alias Pwm1b
C Alias Ocr2

M = 2
'|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
Do
On M Gosub M0 , M1 , M3                                     ', M3
Loop
'|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
M0:
For X = 255 To 1 Step -1
   For Y = 255 To 1 Step -1
      A = Y
      B = X
      Waitus 10
   Next Y
Next X


Return
'********************************
M1:
A = 120
B = 180
C = 100


Return
'********************************
M2:

Return
'********************************
M3:

For X = 0 To 12
   Y = Lookup(x , Speed_a)
   A = Y
   Y = Lookup(x , Speed_b)
   B = Y
   Y = Lookup(x , Speed_c)
   C = Y
   Wait 7
Next X

Return
'================================================
Speed_a:
Data 255 , 255 , 200 , 150 , 100 , 100 , 170 , 190 , 150 , 150 , 170 , 100 , 120,
Speed_b:
Data 150 , 100 , 100 , 250 , 250 , 200 , 100 , 120 , 100 , 250 , 220 , 180 , 180,
Speed_c:
Data 0 , 0 , 250 , 100 , 100 , 100 , 100 , 250 , 0 , 0 , 0 , 0 , 0,
'5-cool 8