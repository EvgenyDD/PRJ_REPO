'0000000000000000000000000000000000000000000000000000000000000000000000000000000
Registr:                                                    'подпрограмма отправки данных в регистр

   Shiftout Portd.6 , Portc.1 , Disp , 1                    'задвигаем данные
   Pulseout Portc , 0 , 5                                   'защелкиваем регистр

Return

'0000000000000000000000000000000000000000000000000000000000000000000000000000000
Adres:                                                      'включение нужной цифры

   Portb = Lookup(digit , Catod)
   Waitus Disp_time                                         'время, в течении которого горит один разряд

Return

'0000000000000000000000000000000000000000000000000000000000000000000000000000000
Resetting:

   Portb.4 = 1 : Portb.5 = 1 : Portb.6 = 1 : Portb.7 = 1

Return
'0000000000000000000000000000000000000000000000000000000000000000000000000000000




'000000000000000000000000000--DISPLAYING--00000000000000000000000000000000000000
Display_main:

 If Mode_n = 0 Then

   Y = I_800 \ 1000 : X = Y                                 '===>I
   Disp = Lookup(x , Disp_tabl2)
      Gosub Registr : Digit = 0 : Gosub Adres : Gosub Resetting

   Y = I_800 Mod 1000 : Y = Y \ 100 : X = Y
   Disp = Lookup(x , Disp_tabl)
      Gosub Registr : Digit = 1 : Gosub Adres : Gosub Resetting

   Y = I_800 Mod 100 : Y = Y \ 10 : X = Y
   Disp = Lookup(x , Disp_tabl)
      Gosub Registr : Digit = 2 : Gosub Adres : Gosub Resetting

   Y = I_800 Mod 10 : X = Y
   Disp = Lookup(x , Disp_tabl)
      Gosub Registr : Digit = 3 : Gosub Adres : Gosub Resetting
 Else

   Y = I_99 \ 1000 : X = Y                                  '===>I
   Disp = Lookup(x , Disp_tabl2)
      Gosub Registr : Digit = 0 : Gosub Adres : Gosub Resetting

   Y = I_99 Mod 1000 : Y = Y \ 100 : X = Y
   Disp = Lookup(x , Disp_tabl)
      Gosub Registr : Digit = 1 : Gosub Adres : Gosub Resetting

   Y = I_99 Mod 100 : Y = Y \ 10 : X = Y
   Disp = Lookup(x , Disp_tabl)
      Gosub Registr : Digit = 2 : Gosub Adres : Gosub Resetting

   Y = I_99 Mod 10 : X = Y
   Disp = Lookup(x , Disp_tabl)
      Gosub Registr : Digit = 3 : Gosub Adres : Gosub Resetting
 End If
   Y = U_real \ 100 : X = Y                                 '===>U
   Disp = Lookup(x , Disp_tabl)
      Gosub Registr : Digit = 5 : Gosub Adres : Gosub Resetting

   Y = U_real Mod 100 : Y = Y \ 10 : X = Y
   Disp = Lookup(x , Disp_tabl2)
      Gosub Registr : Digit = 6 : Gosub Adres : Gosub Resetting

   Y = U_real Mod 10 : X = Y
   Disp = Lookup(x , Disp_tabl)
      Gosub Registr : Digit = 7 : Gosub Adres : Gosub Resetting

Return

'00000000000000000000000000000000000
Display_2:

 Select Case I_string                                       '===>I_analog_meter
''''''''''''''''''''6a54321b
      Case 1 : X = &B00000010
      Case 2 : X = &B00000110
      Case 3 : X = &B00001110
      Case 4 : X = &B00011110
      Case 5 : X = &B00111110
      Case 6 : X = &B10111110
   Case Else : X = &B10010100                               'WTF?
 End Select
 Disp = X
 Gosub Registr : Digit = 8 : Gosub Adres : Gosub Resetting

 Disp = 0                                                   '===>LED Scale
   If Mode_n = 0 Then : Disp = Disp + &B00000001
      Elseif Mode_n = 1 Then : Disp = Disp + &B00010000
      Elseif Mode_n = 2 Then : Disp = Disp + &B00000100
   End If

   If Overload = 1 Then : Disp = Disp + &B10000000 : End If

   If Btn = 1 Then : Disp = Disp + &B00100000 : End If      'blue LED

 Gosub Registr : Digit = 9 : Gosub Adres : Gosub Resetting

Return

'0000000000000000000000000000000000000000000000000000000000000000000000000000000
Display_sub5:

   Y = I_5 / 1000 : X = Y                                   'I_5
   Disp = Lookup(x , Disp_tabl2)
      Gosub Registr : Digit = 0 : Gosub Adres : Gosub Resetting

   Y = I_5 Mod 1000 : Y = Y / 100 : X = Y
   Disp = Lookup(x , Disp_tabl)
      Gosub Registr : Digit = 1 : Gosub Adres : Gosub Resetting

   Y = I_5 Mod 100 : Y = Y / 10 : X = Y
   Disp = Lookup(x , Disp_tabl)
      Gosub Registr : Digit = 2 : Gosub Adres : Gosub Resetting

   Y = I_5 Mod 10 : X = Y
   Disp = Lookup(x , Disp_tabl)
     Gosub Registr : Digit = 3 : Gosub Adres : Gosub Resetting


   Disp = &B10010111                                        '""""U__5""""
      Gosub Registr : Digit = 4 : Gosub Adres : Gosub Resetting
   Disp = &B00000100
      Gosub Registr : Digit = 5 : Gosub Adres : Gosub Resetting
   Disp = &B00000100
      Gosub Registr : Digit = 6 : Gosub Adres : Gosub Resetting
   Disp = &B11110100
      Gosub Registr : Digit = 7 : Gosub Adres : Gosub Resetting

Return

'0000000000000000000000000000000000000000000000000000000000000000000000000000000
Display_sub12:

   Y = I_12 / 1000 : X = Y                                  'I_12
   Disp = Lookup(x , Disp_tabl2)
      Gosub Registr : Digit = 0 : Gosub Adres : Gosub Resetting

   Y = I_12 Mod 1000 : Y = Y / 100 : X = Y
   Disp = Lookup(x , Disp_tabl)
      Gosub Registr : Digit = 1 : Gosub Adres : Gosub Resetting

   Y = I_12 Mod 100 : Y = Y / 10 : X = Y
   Disp = Lookup(x , Disp_tabl)
      Gosub Registr : Digit = 2 : Gosub Adres : Gosub Resetting

   Y = I_12 Mod 10 : X = Y
   Disp = Lookup(x , Disp_tabl)
      Gosub Registr : Digit = 3 : Gosub Adres : Gosub Resetting


   Disp = &B10010111                                        '""""U_12""""
      Gosub Registr : Digit = 4 : Gosub Adres : Gosub Resetting
   Disp = &B00000100
      Gosub Registr : Digit = 5 : Gosub Adres : Gosub Resetting
   Disp = &B00010001
      Gosub Registr : Digit = 6 : Gosub Adres : Gosub Resetting
   Disp = &B01100111
      Gosub Registr : Digit = 7 : Gosub Adres : Gosub Resetting

Return
'0000000000000000000000000000000000000000000000000000000000000000000000000000000
Display_calibrate:

For Y = 0 To 800
   Disp = &B11000110
      Gosub Registr : Digit = 0 : Gosub Adres : Gosub Resetting
   Disp = &B11110011
      Gosub Registr : Digit = 1 : Gosub Adres : Gosub Resetting
   Disp = &B10010010
      Gosub Registr : Digit = 2 : Gosub Adres : Gosub Resetting
   Disp = &B10110110
      Gosub Registr : Digit = 3 : Gosub Adres : Gosub Resetting
   Disp = &B00100010
      Gosub Registr : Digit = 4 : Gosub Adres : Gosub Resetting
   Disp = &B11110011
      Gosub Registr : Digit = 5 : Gosub Adres : Gosub Resetting
   Disp = &B10100110
      Gosub Registr : Digit = 6 : Gosub Adres : Gosub Resetting
   Disp = &B11100110
      Gosub Registr : Digit = 7 : Gosub Adres : Gosub Resetting
Next Y
'0000000000000000000000000000000000000000000000000000000000000000000000000000000








''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
'''''''fagchdeb  segments
Disp_tabl:
Data &B11010111                                             '0
Data &B00010001                                             '1
Data &B01100111                                             '2
Data &B01110101                                             '3
Data &B10110001                                             '4
Data &B11110100                                             '5
Data &B11110110                                             '6
Data &B01010001                                             '7
Data &B11110111                                             '8
Data &B11110101                                             '9

Disp_tabl2:
Data &B11011111                                             '0.
Data &B00011001                                             '1.
Data &B01101111                                             '2.
Data &B01111101                                             '3.
Data &B10111001                                             '4.
Data &B11111100                                             '5.
Data &B11111110                                             '6.
Data &B01011001                                             '7.
Data &B11111111                                             '8.
Data &B11111101                                             '9.

Catod:
Data &B10000100                                             '0   ток
Data &B01010100                                             '1   -
Data &B10010100                                             '2   -
Data &B01100100                                             '3   -
Data &B00110100                                             '4   напряжение
Data &B00100100                                             '5   -
Data &B00010100                                             '6   -
Data &B01110100                                             '7   -
Data &B00000100                                             '8   линейная шкала тока
Data &B01000100                                             '9   вся остальная "иллюминация"