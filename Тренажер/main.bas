$regfile = "m32def.dat"
$crystal = 20000000

Ddra = &B11111111
Ddrb = &B11111111

Dim X As Byte

Dim R1 As Word
Dim R2 As Byte
Dim R3 As Word

Dim Temp As Word
X = 0

'|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
Do
C1:
 R1 = Rnd(160)
 If R1 < 50 Then
  Goto C1
 End If
 R2 = Rnd(10)
C2:
 R3 = Rnd(2000)
 If R3 < 1000 Then
  Goto C2
 End If

 Select Case R2
   Case 0 To 5:
      For X = 2 To 7
       Gosub M0de
       Gosub Registr
       Waitms R1
      Next X
   Case 6 To 10:
      For X = 11 To 15
       Gosub M0de
       Gosub Registr
       Waitms R1
      Next X
'(Case 2:
      For X = 10 To 14
       Gosub M0de
       Gosub Registr
       Waitms R1
      Next X
   Case 3:
      For X = 15 To 19
       Gosub M0de
       Gosub Registr
       Waitms R1
      Next X
')
 End Select
Temp = 1
Gosub Registr

 Waitms R3

Loop

'0000000000000000000000000000000000000000000000000000000000000000000000000000000
Registr:                                                    'подпрограмма отправки данных в регистр
   Shiftout Porta.2 , Porta.0 , Temp , 1                    'задвигаем данные
   Pulseout Portb , 0 , 5                                   'защелкиваем регистр
Return
'================================================
M0de:

Select Case X
  Case 0 : Temp = &B1000000000000000
  Case 1 : Temp = &B1000000000000001
  Case 2 : Temp = &B1000000000000000
  Case 3 : Temp = &B1000000000100000
  Case 4 : Temp = &B1000000000110000
  Case 5 : Temp = &B1000000000111000
  Case 6 : Temp = &B1000000000111100
  Case 7 : Temp = &B1000000000111110
  Case 8 : Temp = &B1000000011111111
  Case 9 : Temp = &B1000000111111111
 Case 10 : Temp = &B1000001111111111
 Case 11 : Temp = &B1000011111000001
 Case 12 : Temp = &B1000111111000001
 Case 13 : Temp = &B1001111111000001
 Case 14 : Temp = &B1011111111000001
 Case 15 : Temp = &B1111111111000001
End Select

Return
'|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||