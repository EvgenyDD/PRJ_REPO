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
Dim Rgb(15) As Byte
Dim Tik As Byte

Dim Tred As Word
Dim Tgrn As Word
Dim Tblu As Word

Dim Red As Integer
Dim Grn As Integer
Dim Blu As Integer

Dim Dred As Integer
Dim Dgrn As Integer
Dim Dblu As Integer

Dim Temp1 As Word
Dim Temp2 As Word
Dim Temp3 As Word
Dim Temp4 As Word
Dim Time_w As Word

Dim Temp As Integer

R Alias Portd.7
G Alias Portd.6
Btn Alias Pind.2
Zvuk Alias Pina.1
Svet Alias Pina.0
'========================Init===================================================
Config Timer0 = Timer , Prescale = 8                        'настраиваем таймер
Enable Timer0                                               'разрешаем работу таймера 0
Enable Interrupts                                           'Разрешаем прерывания
On Timer0 Pwm_out                                           'работа по таймеру 0
Start Timer0                                                'старт таймера 0
'===============================================================================
For X = 1 To 15
  Rgb(x) = 0
Next X
'===========================main cycle  ========================================
Do
  Do
   Gosub Generate
      Temp = Tred - Tgrn : Temp1 = Abs(temp)
      Temp = Tred - Tblu : Temp2 = Abs(temp)
      Temp = Tgrn - Tblu : Temp3 = Abs(temp)
  Loop Until Temp1 < 150 And Temp2 < 2 And Temp3 < 3

Temp = Rnd(100)

If Temp < 40 Then

   Do : Temp4 = Rnd(&Hf) : Loop Until Temp4 = 0 Or Temp4 > 3

   If Temp4 = 1 Then
      Do : Temp4 = Rnd(&Hfff) : Loop Until Temp4 = 0 Or Temp4 > 3276
      Tred = Temp4 * Tred : Tred = Tred / 32767
   End If

   If Temp4 = 2 Then
      Do : Temp4 = Rnd(&Hfff) : Loop Until Temp4 = 0 Or Temp4 > 3276
      Tgrn = Temp4 * Tgrn : Tgrn = Tgrn / 3276
   End If

   If Temp4 = 3 Then
      Do : Temp4 = Rnd(&Hfff) : Loop Until Temp4 = 0 Or Temp4 > 32767
      Tblu = Temp4 * Tblu : Tblu = Tblu / 32767
   End If
End If

Dred = Tred - Red : Dred = Dred / 255
Dgrn = Tgrn - Grn : Dgrn = Dgrn / 255
Dblu = Tblu - Blu : Dblu = Dblu / 255

Do : Temp4 = Rnd(&Hff) : Loop Until Temp4 < 50 Or Temp4 > 100

Time_w = Temp4 * 10 : Time_w = Temp4 / 255

Do
   If Tred <> Red Then : Red = Red + Dred : End If
   If Tgrn <> Grn Then : Grn = Grn + Dgrn : End If
   If Tblu <> Blu Then : Blu = Blu + Dblu : End If

   For X = 1 To 5 : Rgb(x) = Red : Next X
   For X = 6 To 10 : Rgb(x) = Grn : Next X
   For X = 11 To 15 : Rgb(x) = Blu : Next X

   Waitms Time_w                                            'in ms
Loop Until Tred <> Red Or Tgrn <> Grn Or Tblu <> Blu

Temp = Time_w * 255
Waitms Temp                                                 'in ms

Loop
End
'%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
'$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
'###############################################################################
'!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
'0000000000000000000000000000000000000000000000000000000000000000000000000000000
Pwm_out:
Incr Tik
 If Tik = 255 Then
  Tik = 0
  Portc = 1
  Porta = 1
  Portd = &B11110100
 End If
   If Tik > Rgb(1) Then Porta.6 = 0                         'R
   If Tik > Rgb(2) Then Porta.7 = 0
   If Tik > Rgb(3) Then Porta.5 = 0
   If Tik > Rgb(4) Then Porta.4 = 0
   If Tik > Rgb(5) Then Portc.7 = 0
   If Tik > Rgb(6) Then Portd.4 = 0                         'G
   If Tik > Rgb(7) Then Portc.3 = 0
   If Tik > Rgb(8) Then Portc.6 = 0
   If Tik > Rgb(9) Then Porta.3 = 0
  If Tik > Rgb(10) Then Portc.0 = 0
  If Tik > Rgb(11) Then Portc.4 = 0                         'B
  If Tik > Rgb(12) Then Portc.5 = 0
  If Tik > Rgb(13) Then Portd.5 = 0
  If Tik > Rgb(14) Then Portc.1 = 0
  If Tik > Rgb(15) Then Portc.2 = 0
Return
'+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Generate:
Tred = Rnd(255)
Tgrn = Rnd(255)
Tblu = Rnd(255)
Return

'0000000000000000000000000000000000000000000000000000000000000000000000000000000
'0000000000000000000000000000000000000000000000000000000000000000000000000000000

'+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
7seg:
Data &B00100001 , &B11110101 , &B00010011 , &B10010001 , &B11000101 , &B10001001 , &B00001001 , &B11110001 , &B00000001 , &B10000001 , &B01000001 , &B00001101 , &B00001011       'B

Bright:
Data 0 , 6 , 12 , 24 , 47 , 93 , 182 , 255
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