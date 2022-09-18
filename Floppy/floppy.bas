$regfile = "m32def.dat"
$crystal = 20000000

Ddrd = &B11111111

Stepp Alias Portd.1
Steppt Alias Portd.5
Dirr Alias Portd.3
Dirrt Alias Portd.7

Const Lim = 120
Const Half = 500

Dim Count1 As Word
Dim Count2 As Word
Dim Count As Word
Dim X As Word
Dim Q As Byte

Dim Microl As Long

Declare Sub Note_gen(byval Note As Word , Byval Leng As Long)

Config Timer1 = Timer , Prescale = 1024,
'==================================================================
Do
Gosub Melod_1

Loop
'==================================================================

'------------------------------------------------------------------
Melod_1:
'(
 Call Note_gen(262 , Half)
 Call Note_gen(277 , Half)
 Call Note_gen(293 , Half)
 Call Note_gen(311 , Half)
 Call Note_gen(330 , Half)
 Call Note_gen(349 , Half)
 Call Note_gen(370 , Half)
 Call Note_gen(392 , Half)
 Call Note_gen(415 , Half)
')
 Call Note_gen(500 , 1000)
' Wait 1

 'Call Note_gen(466 , Half)
' Call Note_gen(493 , Half)

Return
'------------------------------------------------------------------
Sub Note_gen(byval Note As Word , Byval Leng As Long)
 'Leng = Leng * 1000                                         '1 000 000
 'Microl = 500000 / Note                                     '1 000 частота (в мкс)
 'Count = Leng / Microl                                      '1 000 кол-во повторений toggle
 Microl = 10000
For X = 0 To 1001
   Incr Q
   If Q > Lim Then
        Q = 0
        Toggle Dirr
   End If
 Waitus 1000
 Toggle Stepp
 Toggle Steppt

Next X

End Sub