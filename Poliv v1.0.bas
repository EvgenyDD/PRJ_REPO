$regfile = "m32def.dat"
$crystal = 20000000

Ddrd = &B10110000
Ddrc = &B10010010

Config Adc = single , Prescaler = Auto , Reference = Avcc

Dim X As Byte
Dim Y As Byte
Dim Z As Byte
Dim Aadc As Word

Const Ontime = 18
Const Porog_poliv = 256
Const Porog_sveta = 80
Declare Sub Timing(byval B As Byte )


Led Alias Porta.0
Tap Alias Porta.0
'|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
Do
Aadc = Getadc(0)
Set Led
If Aadc > Porog_poliv Then
   Aadc = Getadc(1)
   If Aadc > Porog_sveta Then
      Gosub Poliv
   End If
Else
   Wait 60
End If

For Z = 1 To 180
   Call Timing(60)
Next Z
Loop
'||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
Poliv:                                                      'занимает 1 минуту
Set Tap
Set Led
   Wait Ontime
Reset Tap
Reset Led

Y = 60 - Ontime
Call Timing(y)

Return
'********************************
Sub Timing(b As Byte )
For X = 1 To B
   Waitms 850
   Set Led
   Waitms 150
Next X
End Sub