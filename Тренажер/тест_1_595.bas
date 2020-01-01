$regfile = "m32def.dat"
$crystal = 20000000

Ddra = &B11111111

Dim M As Byte
Dim X As Byte
Dim Y As Byte
Dim Z As Byte
Dim R1 As Word
Dim R2 As Byte
Dim R3 As Word

Dim Temp As Single

'|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
Do
C1:
 R1 = Rnd(150)
 If R1 < 70 Then
  Goto C1
 End If
 R2 = Rnd(4)
C2:
 R3 = Rnd(2000)
 If R3 < 100 Then
  Goto C2
 End If

 Select Case R2
   Case 0:
      For X = 0 To 4
       Gosub M0de
       Gosub Registr
       Waitms R1
      Next X
   Case 1:
      For X = 5 To 9
       Gosub M0de
       Gosub Registr
       Waitms R1
      Next X
   Case 2:
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
 End Select
Temp = 1
Gosub Registr

 Waitms R3
Loop
'|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

'(M1:
 Temp = Lookup(1 , Mass)

Return
')
'0000000000000000000000000000000000000000000000000000000000000000000000000000000
Registr:                                                    'подпрограмма отправки данных в регистр
   Shiftout Porta.2 , Porta.0 , Temp , 1                    'задвигаем данные
   Pulseout Porta , 4 , 5                                   'защелкиваем регистр
Return
'================================================
M0de:

Select Case X
Case 0 : Temp =   &B100000000000000000000001
  Case 1 : Temp = &B100000000000000000000011
  Case 2 : Temp = &B100000000000000000000111
  Case 3 : Temp = &B100000000000000000001111
  Case 4 : Temp = &B100000000000000000011111
Case 5 : Temp = &B1000000000000000100
  Case 6 : Temp = &B1000000000000000110
  Case 7 : Temp = &B1000000000000000111
  Case 8 : Temp = &B1000000000000001111
  Case 9 : Temp = &B1000000000000011111
Case 10 : Temp = &B10000000010000
  Case 11 : Temp = &B10000000011000
  Case 12 : Temp = &B10000000011100
  Case 13 : Temp = &B10000000011110
  Case 14 : Temp = &B10000000011111
Case 15 : Temp = &B100000001
  Case 16 : Temp = &B100000011
  Case 17 : Temp = &B100000111
  Case 18 : Temp = &B100001111
  Case 19 : Temp = &B100011111
End Select

Return
'|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
