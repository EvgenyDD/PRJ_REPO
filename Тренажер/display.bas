Display_out:
  If M = 0 Then
      Y = Adc_d(1) / 10 : X = Y                             'Y = Ch / 10 : X = Y                                   '                             'вывод начального...
      If Adc_d_f(1) = 0 Then : Disp1 = Lookup(x , Disp_tabl_dot) : Else : Disp1 = Lookup(x , Disp_tabl) : End If
      Y = Adc_d(3) / 10 : X = Y
      If Adc_d_f(3) = 0 Then : Disp2 = Lookup(x , Disp_tabl_dot) : Else : Disp2 = Lookup(x , Disp_tabl) : End If
      Gosub Registr_up : Gosub Registr_dn : Digit = 0 : Gosub Adres : Gosub Resetting

      Y = Adc_d(1) Mod 10 : X = Y                           'Y = Ch Mod 10 : X = Y                                 'Y = Adc_d(1) Mod 10 : X = Y                           '...и конечного времени удара (с потенциометров)
      Disp1 = Lookup(x , Disp_tabl)
      Y = Adc_d(3) Mod 10 : X = Y
      Disp2 = Lookup(x , Disp_tabl)
      Gosub Registr_up : Gosub Registr_dn : Digit = 1 : Gosub Adres : Gosub Resetting

      Y = Adc_d(2) / 10 : X = Y                             'вывод начального...
      If Adc_d_f(2) = 0 Then : Disp1 = Lookup(x , Disp_tabl_dot) : Else : Disp1 = Lookup(x , Disp_tabl) : End If
      Y = Adc_d(4) / 10 : X = Y
      If Adc_d_f(4) = 0 Then : Disp2 = Lookup(x , Disp_tabl_dot) : Else : Disp2 = Lookup(x , Disp_tabl) : End If
      Gosub Registr_up : Gosub Registr_dn : Digit = 2 : Gosub Adres : Gosub Resetting

      Y = Adc_d(2) Mod 10 : X = Y                           '...и конечного времени паузы (с потенциометров)
      Disp1 = Lookup(x , Disp_tabl)
      Y = Adc_d(4) Mod 10 : X = Y
      Disp2 = Lookup(x , Disp_tabl)
      Gosub Registr_up : Gosub Registr_dn : Digit = 3 : Gosub Adres : Gosub Resetting

      Gosub Led16
      Gosub Registr_up : Gosub Registr_dn : Digit = 4 : Gosub Adres : Gosub Resetting


   '///////////////////////////////
   Elseif M = 1 Then                                        'Режим чтенья программ

      Y = N_prog / 10 : X = Y
      Disp1 = Lookup(x , Disp_tabl)
      If Flag_mode1 = 1 Then
         If N_mode = 0 Then : X = 10 : Elseif N_mode = 1 Then : X = 11 : Elseif N_mode = 2 Then : X = 15 : End If
         Disp2 = Lookup(x , Disp_tabl)
      Else : Disp2 = 0 : End If
   Gosub Registr_up : Gosub Registr_dn : Digit = 0 : Gosub Adres : Gosub Resetting

      Y = N_prog Mod 10 : X = Y                             '
      Disp1 = Lookup(x , Disp_tabl)
      Disp2 = 0                                             'пробел (3 цифра)
   Gosub Registr_up : Gosub Registr_dn : Digit = 1 : Gosub Adres : Gosub Resetting


      If Flag_mode1 = 1 Then
         Y = N_step / 10 : X = Y
         If N_step <> 0 Then : Disp1 = Lookup(x , Disp_tabl) : Else : Disp1 = 0 : End If
         Y = N_time / 10 : X = Y
         If N_mode <> 2 Then : If N_time_f = 1 Then : Disp2 = Lookup(x , Disp_tabl_dot) : Else : Disp2 = Lookup(x , Disp_tabl) : End If : Else : Disp2 = 0 : End If
      Else : Disp1 = 0 : Disp2 = 0 : End If
   Gosub Registr_up : Gosub Registr_dn : Digit = 2 : Gosub Adres : Gosub Resetting

      If Flag_mode1 = 1 Then
         Y = N_step Mod 10 : X = Y
         If N_step <> 0 Then : Disp1 = Lookup(x , Disp_tabl) : Else : Disp1 = 0 : End If
         Y = N_time Mod 10 : X = Y
         If N_mode <> 2 Then : Disp2 = Lookup(x , Disp_tabl) : Else : Disp2 = 0 : End If
      Else : Disp1 = 0 : Disp2 = 0 : End If
   Gosub Registr_up : Gosub Registr_dn : Digit = 3 : Gosub Adres : Gosub Resetting

      If N_mode <> 2 Then : Gosub Led16
      Gosub Registr_up : Gosub Registr_dn : Digit = 4 : Gosub Adres : Gosub Resetting : End If

   '///////////////////////////////
   Elseif M = 2 Then                                        'Режим программирования

      Y = N_prog / 10 : X = Y
      Disp1 = Lookup(x , Disp_tabl)
      If N_mode = 0 Then : X = 10 : Elseif N_mode = 1 Then : X = 11 : Elseif N_mode = 2 Then : X = 12 : End If
      Disp2 = Lookup(x , Disp_tabl)
   Gosub Registr_up : Gosub Registr_dn : Digit = 0 : Gosub Adres : Gosub Resetting

      Y = N_prog Mod 10 : X = Y                             '
      Disp1 = Lookup(x , Disp_tabl)
      If N_mode = 2 Then : Disp2 = Lookup(13 , Disp_tabl) : Else : Disp2 = 0 : End If       'пробел (3 цифра)
   Gosub Registr_up : Gosub Registr_dn : Digit = 1 : Gosub Adres : Gosub Resetting

      Y = N_step / 10 : X = Y
      Disp1 = Lookup(x , Disp_tabl)
      Y = N_time / 10 : X = Y
      If N_time_f = 1 Then : Disp2 = Lookup(x , Disp_tabl_dot) : Else : Disp2 = Lookup(x , Disp_tabl) : End If
      If N_mode = 2 Then : Disp2 = Lookup(14 , Disp_tabl) : End If
   Gosub Registr_up : Gosub Registr_dn : Digit = 2 : Gosub Adres : Gosub Resetting

      Y = N_step Mod 10 : X = Y
      Disp1 = Lookup(x , Disp_tabl)
      Y = N_time Mod 10 : X = Y
      If N_mode <> 2 Then : Disp2 = Lookup(x , Disp_tabl) : Else : Disp2 = 0 : End If
   Gosub Registr_up : Gosub Registr_dn : Digit = 3 : Gosub Adres : Gosub Resetting

      Gosub Led16
      Gosub Registr_up : Gosub Registr_dn : Digit = 4 : Gosub Adres : Gosub Resetting

      '///////////////////////////////
   Elseif M = 3 Then                                        'Режим M_all

      Y = N_prog / 10 : X = Y
      Disp1 = Lookup(x , Disp_tabl)
      If Flag_mode1 = 1 Then
         If N_mode = 0 Then : X = 10 : Elseif N_mode = 1 Then : X = 11 : Elseif N_mode = 2 Then : X = 15 : End If
         Disp2 = Lookup(x , Disp_tabl)
      Else : Disp2 = 0 : End If
   Gosub Registr_up : Gosub Registr_dn : Digit = 0 : Gosub Adres : Gosub Resetting

      Y = N_prog Mod 10 : X = Y                             '
      Disp1 = Lookup(x , Disp_tabl)
      Disp2 = 0                                             'пробел (3 цифра)
   Gosub Registr_up : Gosub Registr_dn : Digit = 1 : Gosub Adres : Gosub Resetting


         Y = N_repeat / 10 : X = Y
         Disp1 = Lookup(x , Disp_tabl)
      If Flag_mode1 = 1 Then
         Y = N_time / 10 : X = Y
                 If N_mode <> 2 Then : If N_time_f = 1 Then : Disp2 = Lookup(x , Disp_tabl_dot) : Else : Disp2 = Lookup(x , Disp_tabl) : End If : Else : Disp2 = 0 : End If
      Else : Disp2 = 0 : End If
   Gosub Registr_up : Gosub Registr_dn : Digit = 2 : Gosub Adres : Gosub Resetting


         Y = N_repeat Mod 10 : X = Y
         Disp1 = Lookup(x , Disp_tabl)
      If Flag_mode1 = 1 Then
         Y = N_time Mod 10 : X = Y
         If N_mode <> 2 Then : Disp2 = Lookup(x , Disp_tabl) : Else : Disp2 = 0 : End If
      Else : Disp2 = 0 : End If
   Gosub Registr_up : Gosub Registr_dn : Digit = 3 : Gosub Adres : Gosub Resetting

      Gosub Led16
      Gosub Registr_up : Gosub Registr_dn : Digit = 4 : Gosub Adres : Gosub Resetting

   '///////////////////////////////

   End If

Return
'=================================================================
Registr_up:
   Shiftout Portg.4 , Portb.7 , Disp1 , 1 : Pulseout Portg , 3 , 5
Return
Registr_dn:
   Shiftout Portb.6 , Portb.5 , Disp2 , 1 : Pulseout Portg , 3 , 5
Return
'=================================================================
Adres:                                                      'включение нужной цифры
   Portb = Lookup(digit , Catod)
   Waitms Disp_time                                         'время, в течении которого горит один разряд
Return
'=================================================================
Resetting:
   Portb.0 = 1 : Portb.1 = 1 : Portb.2 = 1
Return
'=================================================================

Catod:
Data &B00000000
Data &B00000001
Data &B00000010
Data &B00000011
Data &B00000100

Disp_tabl:
'''''''EDHCGBFA
Data &B11010111                                             '0
Data &B00010100                                             '1
Data &B11001101                                             '2
Data &B01011101                                             '3
Data &B00011110                                             '4
Data &B01011011                                             '5
Data &B11011011                                             '6
Data &B00010101                                             '7
Data &B11011111                                             '8
Data &B01011111                                             '9
Data &B10010111
Data &B01011110
Data &B11001011                                             'E
Data &B10011000                                             'n
Data &B11011100                                             'd
Data &B01000000                                             'd
Data 0

Disp_tabl_dot:
'''''''EDHCGBFA
Data &B11110111                                             '0
Data &B00110100                                             '1
Data &B11101101                                             '2
Data &B01111101                                             '3
Data &B00111110                                             '4
Data &B01111011                                             '5
Data &B11111011                                             '6
Data &B00110101                                             '7
Data &B11111111                                             '8
Data &B01111111                                             '9
Data &B11111111




Led16:
Disp1 = 0
Disp2 = 0
For X = 1 To 16
If Led_on(x) = 1 Then
 '     Q = Lookup(x , Shift_table)
 Select Case X
      Case 1 : Disp1 = Disp1 + &B00000001
      Case 2 : Disp1 = Disp1 + &B01000000
      Case 3 : Disp2 = Disp2 + &B00000100
      Case 4 : Disp2 = Disp2 + &B01000000

      Case 5 : Disp1 = Disp1 + &B00000010
      Case 6 : Disp1 = Disp1 + &B00100000
      Case 7 : Disp2 = Disp2 + &B00000010
      Case 8 : Disp2 = Disp2 + &B00100000

      Case 9 : Disp1 = Disp1 + &B00000100
     Case 10 : Disp1 = Disp1 + &B00010000
     Case 11 : Disp2 = Disp2 + &B00000001
     Case 12 : Disp2 = Disp2 + &B00010000

     Case 13 : Disp1 = Disp1 + &B10000000
     Case 14 : Disp1 = Disp1 + &B00001000
     Case 15 : Disp2 = Disp2 + &B10000000
     Case 16 : Disp2 = Disp2 + &B00001000
 End Select

      'Disp1 = Disp1 + Q
End If

Next X
'(
Disp2 = 0
For X = 9 To 16
   If Led_on(x) = 1 Then
      W = X - 8
      Q = Lookup(w , Shift_table)
      Disp2 = Disp2 + Q
   End If
Next X
')
Return
'(
Shift_table:
Data &B00000000
Data &B10000000
Data &B01000000
Data &B00100000
Data &B00010000
Data &B00001000
Data &B00000100
Data &B00000010
Data &B00000001
')