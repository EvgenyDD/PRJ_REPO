$regfile = "m32def.dat"
$crystal = 20000000

Ddrd = &B10110000
Portd = &B00001110
Ddrc.7 = 1
Portc.1 = 0
Ddra = &B00000000
Porta = &B01010101

Config Int1 = Low Level
Config Timer1 = Pwm , Pwm = 8 , Compare A Pwm = Clear Down , Compare B Pwm = Clear Down , Prescale = 1
Config Timer2 = Pwm , Prescale = 1 , Pwm = On , Compare Pwm = Clear Up
Dim M As Byte
Dim X As Byte
Dim Y As Byte
Dim Z As Byte
Dim Wms As Word

A Alias Pwm1a
B Alias Pwm1b
C Alias Ocr2

On Int1 Strobo


Wms = 200
M = 0
'|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
Do
'(If Pina.6 = 0 Then
   Waitms 50
   Incr M
   If M = 3 Then
   M = 0
   End If
End If
')
 Xx:
If Pind.1 = 0 Then
   Gosub Flash
   Goto Xx
End If

If Pind.3 = 0 Then
   Gosub Strobo
   Goto Xx
End If

On M Gosub M0 , M1 , M2                                     ', M3
Loop
'|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
M1:
A = 255
B = 0
C = 0
Waitms 200
A = 0
B = 0
C = 0
Waitms 200
A = 0
B = 255
C = 0
Waitms 200
A = 0
B = 0
C = 0
Waitms 200
A = 255
B = 255
C = 0
Waitms 200
A = 0
B = 0
C = 0
Waitms 200
A = 0
B = 0
C = 255
Waitms 200
A = 0
B = 0
C = 0
Waitms 200
A = 255
B = 0
C = 255
Waitms 200
A = 0
B = 0
C = 0
Waitms 200
A = 0
B = 255
C = 255
Waitms 200
A = 0
B = 0
C = 0
Waitms 200
A = 255
B = 255
C = 255
Waitms 200
A = 0
B = 0
C = 0
Waitms 200
Return

Return
'********************************
M0:
'If Pina.2 = 0 Then
 'wms=wms+30
A = 255
B = 0
C = 0
Waitms 200
A = 0
B = 255
C = 0
Waitms 200
A = 255
B = 255
C = 0
Waitms 200
A = 0
B = 0
C = 255
Waitms 200
A = 255
B = 0
C = 255
Waitms 200
A = 0
B = 255
C = 255
Waitms 200
A = 255
B = 255
C = 255
Waitms 200
Return
'********************************
Flash:
If Pind.2 = 0 Then
A = 255
B = 255
C = 255
Else

A = 0
B = 0
C = 0
End If
Return
'''''''''''''
Strobo:
A = 255
B = 255
C = 255
Waitms 50
A = 0
B = 0
C = 0
Waitms 50

Return
''''''''''''''
M3:

Return
M2:

Return
'********************************
