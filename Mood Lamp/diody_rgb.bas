$regfile = "m8def.dat"
$crystal = 8000000
'--------------------------Порты выхода-------------------------
Config Pinc.6 = Output 
Config Pind.0 = Output 
Config Pind.1 = Output 
Config Pind.2 = Output 
Config Pind.3 = Output 
Config Pind.4 = Output 
Config Pinb.6 = Output 
Config Pinb.7 = Output 
Config Pind.5 = Output 
Config Pind.6 = Output 
Config Pind.7 = Output 
Config Pinb.0 = Output 
Config Pinb.1 = Output 
Config Pinb.2 = Output 
Config Pinb.3 = Output 
Config Pinb.4 = Output 
Config Pinb.5 = Output 
Config Pinc.0 = Output
'------------------------- Названия LED---------------------------------
Led1_blue Alias Portc.0
Led1_green Alias Portb.5
Led1_red Alias Portb.4
'=
Led2_blue Alias Portb.3
Led2_green Alias Portb.2
Led2_red Alias Portb.1
'=
Led3_blue Alias Portd.6
Led3_green Alias Portd.7
Led3_red Alias Portb.0
'=
Led4_blue Alias Portb.6
Led4_green Alias Portb.7
Led4_red Alias Portd.5
'=
Led5_blue Alias Portd.2
Led5_green Alias Portd.3
Led5_red Alias Portd.4
'=
Led6_blue Alias Portc.6
Led6_green Alias Portd.0
Led6_red Alias Portd.1
'-------------------------Высокий сигнал портов(њwieci dla stanu niskiego)-------
Led1_blue = 1
Led1_green = 1
Led1_red = 1
Led2_blue = 1
Led2_green = 1
Led2_red = 1
Led3_blue = 1
Led3_green = 1
Led3_red = 1
Led4_blue = 1
Led4_green = 1
Led4_red = 1
Led5_blue = 1
Led5_green = 1
Led5_red = 1
Led6_blue = 1
Led6_green = 1
Led6_red = 1
'---------------------------Объявление переменных-----------------------------
Dim I As Byte , J As Byte , K As Byte , L As Byte
Dim Led(18) As Integer
Dim Change_blue As Integer , Zmien_green As Integer , Zmien_red As Integer
Declare Sub Software_PWM
Declare Sub LED_Value
Dim Zmiana_kolejnosci As Bit : Mode_LED = 0
For J = 1 To 6 Step 1
   Led(j) = 0
   Led(j + 6) = 150
   Led(j + 12) = 150
   Call Software_PWM
Next

'---------------------------Главная программа--------------------------------
Do
 Change_blue = 0 : Change_green = 0 : Change_red = -1          'зелёный (жёлтый-красный)
 Call LED_Value
 Change_blue = 1 : Change_green = 0 : Change_red = 0           'морской (зелёный-голубой)
 Call LED_Value
 Change_blue = 0 : Change_green = -1 : Change_red = 0          'голубой
 Call LED_Value
 Change_blue = 0 : Change_green = 0 : Change_red = 1           'фиолетовый (голубой-красный)
 Call LED_Value
 Change_blue = -1 : Change_green = 0 : Change_red = 0          'красный
 Call LED_Value
 Change_blue = 0 : Change_green = 1 : Change_red = 0           'золотой
 Call LED_Value
Loop
End
'---------------------------Подпрограммы------------------------------------------
Sub Software_PWM
   For I = 1 To 150 Step 1
   If I >= Led(1) Then Led1_blue = 1 Else Led1_blue = 0
   If I >= Led(7) Then Led1_green = 1 Else Led1_green = 0
   If I >= Led(13) Then Led1_red = 1 Else Led1_red = 0

   If I >= Led(2) Then Led2_blue = 1 Else Led2_blue = 0
   If I >= Led(8) Then Led2_green = 1 Else Led2_green = 0
   If I >= Led(14) Then Led2_red = 1 Else Led2_red = 0

   If I >= Led(3) Then Led3_blue = 1 Else Led3_blue = 0
   If I >= Led(9) Then Led3_green = 1 Else Led3_green = 0
   If I >= Led(15) Then Led3_red = 1 Else Led3_red = 0

   If I >= Led(4) Then Led4_blue = 1 Else Led4_blue = 0
   If I >= Led(10) Then Led4_green = 1 Else Led4_green = 0
   If I >= Led(16) Then Led4_red = 1 Else Led4_red = 0

   If I >= Led(5) Then Led5_blue = 1 Else Led5_blue = 0
   If I >= Led(11) Then Led5_green = 1 Else Led5_green = 0
   If I >= Led(17) Then Led5_red = 1 Else Led5_red = 0

   If I >= Led(6) Then Led6_blue = 1 Else Led6_blue = 0
   If I >= Led(12) Then Led6_green = 1 Else Led6_green = 0
   If I >= Led(18) Then Led6_red = 1 Else Led6_red = 0
   Next
   Return
End Sub
'-----------------------PWM для каждого LED--------------------
Sub LED_Value
 If Mode_LED = 0 Then
 For J = 1 To 6 Step 1
  For K = 0 To 150 Step 1
    Led(j) = Led(j) + Change_blue
    Led(j + 6) = Led(j + 6) + Change_green
    Led(j + 12) = Led(j + 12) + Change_red
    For L = 1 To 10 Step 1
    Call Software_PWM
    Next
  Next
 Next
 Mode_LED = 1
 Else
 For J = 6 To 1 Step -1
  For K = 0 To 150 Step 1
   Led(j) = Led(j) + Change_blue
   Led(j + 6) = Led(j + 6) + Change_green
   Led(j + 12) = Led(j + 12) + Change_red
    For L = 1 To 10 Step 1
    Call Software_PWM
    Next
  Next
 Next
 Mode_LED = 0
 End If
 Return
End Sub