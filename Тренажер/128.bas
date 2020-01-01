$regfile = "m128def.dat"
$crystal = 16000000
$include "init.bas"

Dim M As Byte                                               'номер режима (0 - рандом, 1 - play, 2 - prog)
M = 0
Dim M_last As Byte
M_last = 0
Dim N_prog As Byte                                          'номер программы

'управление выводом светодиодных полос

'STRINGS
Dim Ee_mode(25) As Eram String * 50 At 0
Dim Ee_mass(25) As Eram String * 50 At 1300
Dim Ee_time(25) As Eram String * 50 At 2600
Dim Flag_dot_ee As Eram Byte At 3900

Dim E_mode(25) As String * 50
Dim E_mass(25) As String * 50
Dim E_time(25) As String * 50
Dim Str_temp As String * 1

Dim Led_on(16) As Byte                                      'массив с включенными каналами
Const Disp_time = 1                                         'время, мс, в течении которого горит один разряд
Const Butn_opros = 10                                       'в теч X циклов мэйна не будут опрашиваться кнопки после нажатия
Const N_step_lim = 51                                       'максимальный доступный шаг

Dim Disp_b As Byte
Dim Disp_w As Word

'управление выводом светодиодных полос
Dim N_on As Byte                                            'номер включенного светодиода (1 - 5) ПРИ ВЫВОДЕ НА РЕГИСТРЫ
Dim Ch As Byte                                              'номер канала (1 - 16) ПРИ ВЫВОДЕ НА РЕГИСТРЫ
Dim Flag_dot As Byte : Flag_dot = Flag_dot_ee               '1-режим накопления, 0-без накопления включение удара

Dim X As Byte
Dim Q As Byte
Dim W As Byte
Dim Y As Word
Dim Z As Word
Dim R As Byte

'работа со STRING
Dim N_step As Byte


Dim Flag_set As Byte                                        'флаг режима ввода(удар, пауза, end) в PROG
Dim N_mode As Byte
Dim N_time As Byte
Dim N_time_f As Bit
Dim N_mass As Byte
Dim N_repeat As Byte
Dim N_repeat_copy As Byte

N_repeat = 1

N_step = 1
N_prog = 1

Dim Flag_int_buttn As Byte
Dim Flag_int_buttn_count As Byte

Dim Disp1 As Byte
Dim Disp2 As Byte
Dim Digit As Byte

Dim Temp As Integer

Dim Tim0count As Byte                                       'подсчет времени на 0 таймере
Dim Strike As Byte                                          'кол-во итераций по 0.02 секунд
Dim Pause As Byte
Dim Flag As Bit                                             'режим окончания паузы или удара
Dim Flag_strike_pause As Bit                                'пауза или удар выполняются
Dim Flag_m_all As Bit                                       'флаг установки режима M_ALL
Dim Count1 As Byte
Dim Countm As Byte
Pause = 1 : Strike = 1

Dim Flag_mode1 As Bit                                       'флаг отображения детальной информации в режиме PLAY
Flag_mode1 = 1

Dim Adc_r(4) As Byte
Dim Adc_d(4) As Byte
Dim Adc_d_f(4) As Byte                                      'флаг десятичной точки

Dim Aaa As Byte : Dim Bbb As Word : Aaa = 0 : Bbb = 0       'для инициализации регистров

Waitms 200                                                  'на всякий случай
For X = 1 To 16 : Led_on(x) = 1 : Next X                    'по умолчанию в random все каналы включены
If Pind.3 = 0 Then : Gosub Out_test : End If                'тест работы LED_string при нажатии на кнопку OPTION
Gosub Init_out                                              'инициализация 595
Flag_dot = Flag_dot_ee                                      'копирование режима работы(бегущ/накоплен) из EEPROM
Flag_int_buttn_count = Butn_opros
Flag_int_buttn = 0

'###############################################################################
'8888888888888888888888888888888888888888888888888888888888888888888888888888888
Do

   If Flag_int_buttn = 1 Then                               'DEBOUNCE for INT buttons
      Disable Int4 : Disable Int5 : Disable Int6 : Disable Int7 : Disable Int0       ': Disable Int3
      Decr Flag_int_buttn_count
      If Flag_int_buttn_count = 0 Then : Flag_int_buttn_count = Butn_opros : Flag_int_buttn = 0
         Enable Int4 : Enable Int5 : Enable Int6 : Enable Int7 : Enable Int0       ': Enable Int3
      End If
   End If
   If Flag_int_buttn = 2 Then                               'DEBOUNCE for OPTION_btn
      Decr Flag_int_buttn_count
      If Flag_int_buttn_count = 0 Then : Flag_int_buttn_count = Butn_opros : Flag_int_buttn = 0 : End If
   End If

   Gosub Check                                              'переключатель каналов - 3 режима
   On M Gosub M_random , M_play , M_prog , M_all            'выбор программы работы
   Gosub Display_out                                        'вывод на дисплей


Loop
'8888888888888888888888888888888888888888888888888888888888888888888888888888888
'###############################################################################
 $include "output.bas"
'###############################################################################
Int_0:
 Incr Tim0count
 If Tim0count = 157 Then                                    'прерывание (157 - 50 Hz)
      Tim0count = 0

 'Toggle Portf.0
  If Flag = 1 Then
   If Flag_strike_pause = 0 Then
      '_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _            'если режим "пауза"
    Decr Count1
      If Count1 = 0 Then
         If Countm = 0 Then
            Flag_strike_pause = 1 : Flag = 0 : Countm = 5 : Gosub Init_out       ': Portf.2 = 0
         Else
            Decr Countm : Count1 = Pause
         End If
      End If
      '_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
   Else
      '/ / / / / / / / / / / / / / / / / / /                'если режим "удар"
    Decr Count1
      If Count1 = 0 Or Count1 = 255 Then
         If Countm = 0 Then
            Flag_strike_pause = 0 : Flag = 0 : Countm = 5 : Gosub Init_out       ': Portf.2 = 1
         Else
            Decr Countm : Count1 = Strike
            N_on = 5 - Countm : Gosub Out_out
         End If
      End If
      '/ / / / / / / / / / / / / / / / / / /
   End If
  End If
'  Gosub Init_out
 End If
Return
'###############################################################################
Check:
Disable Interrupts
   If Pind.3 = 0 And Flag_int_buttn = 0 Then : Gosub Button_21 : End If       'OPTION btn

  If Ping.1 = 0 Then
      M = 0                                                 'RAND mode - GRN LED
   Else
      If Ping.0 = 1 Then
         If Flag_m_all = 0 Then : M = 1                     'PLAY mode - RED & WHT LED
         Else : M = 3
         End If
'M = 1
      Else : M = 2 : End If                                 'PROG mode - RED & BLU LED
   End If

       If M = 0 Then : Portd.5 = 0 : Portd.6 = 0 : Portd.7 = 0
   Elseif M = 1 Then : Portd.5 = 1 : Portd.6 = 1 : Portd.7 = 0
   Elseif M = 3 Then : Portd.5 = 1 : Portd.6 = 1 : Portd.7 = 1
   Elseif M = 2 Then : Portd.5 = 1 : Portd.6 = 0 : Portd.7 = 1
   End If


   If M_last = 2 And M <> 2 Then :                          'если из режима PROG переключаемся в другой режим
      Stop Timer0 : Stop Timer1
      Ee_mode(n_prog) = E_mode(n_prog)
      Ee_mass(n_prog) = E_mass(n_prog)
      Ee_time(n_prog) = E_time(n_prog)
      N_step = 1
      Gosub Init_out
      Flag_dot = Flag_dot_ee
      Portf.2 = 0 : Portf.0 = 0
      Flag = 0
   End If
  '000
   If M_last <> 2 And M = 2 Then :                          'если из другого режима переключаемся в режим PROG
      Stop Timer0 : Stop Timer1
      E_mode(n_prog) = Ee_mode(n_prog)
      E_mass(n_prog) = Ee_mass(n_prog)
      E_time(n_prog) = Ee_time(n_prog)
      N_step = 1
   End If
  '000
   If M_last <> 0 And M = 0 Then :                          'если из другого режима переключаемся в режим RAND
      Stop Timer0 : Stop Timer1

   End If
  '000
   If M_last <> 1 And M = 1 Then :                          'если из другого режима переключаемся в режим PLAY
      Stop Timer0 : Stop Timer1
      E_mode(n_prog) = Ee_mode(n_prog)
      E_mass(n_prog) = Ee_mass(n_prog)
      E_time(n_prog) = Ee_time(n_prog)
      N_step = 0                                            '!!!
      Count1 = 1 : Countm = 1
      Flag = 0
      Gosub Backup
   End If
  '000
   If M_last <> 3 And M = 3 Then :                          'если из другого режима переключаемся в режим PLAY_ALL
      Stop Timer0 : Stop Timer1
      For X = 1 To 25                                       '
       E_mode(x) = Ee_mode(x)
       E_mass(x) = Ee_mass(x)
       E_time(x) = Ee_time(x)
      Next X
      N_step = 0
      N_repeat_copy = N_repeat
      Flag = 0
      N_prog = 1

   End If
  '000
   If M_last <> M Then : Gosub Backup : M_last = M : End If
Enable Interrupts
Return
'$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$








'$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
M_random:
   Gosub Adc_read

 If Flag = 0 Then                                           'если флаг - сброшен - режим закончился
   If Flag_strike_pause = 1 Then
      Y = Adc_r(2) - Adc_r(1)
      If Y = 0 Then
            Strike = Adc_r(1)
      Else
            Y = Rnd(y)
            Strike = Adc_r(1) + Y
      End If
      Count1 = Strike
      Gosub Randomgen
   Else
      Y = Adc_r(4) - Adc_r(3)
      If Y = 0 Then
            Pause = Adc_r(3)
      Else
            Y = Rnd(y)
            Pause = Adc_r(3) + Y
      End If
      Count1 = Pause
   End If
   Flag = 1
 End If

   Start Timer0 : Start Timer1
   Enable Int4 : Enable Int5 : Enable Int6 : Enable Int7
Return






'###############################################################################
M_play:
Stop Timer0 : Stop Timer1
Disable Int4 : Disable Int5 : Disable Int6 : Disable Int7

If Flag = 0 Then                                            'если шаг закончил выполняться, то надо загрузить данные слеующего шага
   Incr N_step
   If N_step = N_step_lim Then
Back:
      N_step = 1
   End If
   'READING STRING//////////////////////////////////////////////////////////////
   Disable Interrupts
      Str_temp = Mid(e_mode(n_prog) , N_step , 1)           'чтенье режима работы
      If Str_temp = "0" Then
         N_mode = 0 : Flag_strike_pause = 0
      Elseif Str_temp = "1" Then
         N_mode = 1 : Flag_strike_pause = 1
      Elseif Str_temp = "2" Then
         N_mode = 2
         If N_step = 1 Then : Goto Labl1
         Else : : Goto Back : End If
      End If

      Str_temp = Mid(e_time(n_prog) , N_step , 1)           'чтенье времени паузы\удара
      Gosub Choose
      If N_mode = 1 Then
         Count1 = Lookup(r , Mass1 )
         Strike = Count1
         N_time = Count1 : N_time_f = 1
      Else
         Count1 = Lookup(r , Mass2 )
         Pause = Count1
         If Count1 > 99 Then                                'вывод времени на экран  (с запятой или без)
            N_time = Count1 / 10 : N_time_f = 0
         Else
            N_time = Count1 : N_time_f = 1
         End If
      End If

      For Q = 1 To 16 : Led_on(q) = 0 : Next Q
      If N_mode = 1 Then
      Str_temp = Mid(e_mass(n_prog) , N_step , 1)           'чтенье канала удара
      Gosub Choose
   '   N_mass = X + 1
      Led_on(r) = 1
      Ch = R
      End If
   Enable Interrupts
   'READING STRING//////////////////////////////////////////////////////////////
   'на выходе: N_mode, n_time

Labl2:
   Flag = 1                                                 'установка флага переопределения на 1(false)
End If
Start Timer0 : Start Timer1
Return
'$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
Labl1:
   N_step = 0
   Goto Labl2







'$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
M_prog:
Stop Timer0 : Stop Timer1
   Gosub Init_out
   Flag_dot = 1                                             'чтобы при программировании отображались все ЛЕД руки

   Str_temp = Mid(e_mode(n_prog) , N_step , 1)              'чтенье режима работы
      If Str_temp = "0" Then
         N_mode = 0 : Flag_strike_pause = 0
      Elseif Str_temp = "1" Then
         N_mode = 1 : Flag_strike_pause = 1
      Elseif Str_temp = "2" Then
         N_mode = 2
      End If

      Str_temp = Mid(e_time(n_prog) , N_step , 1)           'чтенье времени паузы\удара
      Gosub Choose
      If N_mode = 1 Then
         Count1 = Lookup(r , Mass1 )
         N_time = Count1 : N_time_f = 1
      Else
         Count1 = Lookup(r , Mass2 )
         If Count1 > 99 Then                                'вывод времени на экран  (с запятой или без)
            N_time = Count1 / 10 : N_time_f = 0
         Else
            N_time = Count1 : N_time_f = 1
         End If
      End If

      For Q = 1 To 16 : Led_on(q) = 0 : Next Q
      If N_mode = 1 Then
      Str_temp = Mid(e_mass(n_prog) , N_step , 1)           'чтенье канала удара
      Gosub Choose
   '   N_mass = X + 1
      Led_on(r) = 1
      Ch = R : N_on = 5 : Gosub Out_out
      End If


   If Flag_set = 1 Then
      Y = Getadc(4) : Y = Y / 64 : Q = Y
      Gosub Choose_rev
      Mid(e_time(n_prog) , N_step , 1) = Str_temp
      Mid(e_mode(n_prog) , N_step , 1) = "0"
   Elseif Flag_set = 2 Then
      Y = Getadc(6) : Y = Y / 64 : Q = Y
      Gosub Choose_rev
      Mid(e_time(n_prog) , N_step , 1) = Str_temp
      Mid(e_mode(n_prog) , N_step , 1) = "1"
      If N_mass > 16 Or N_mass < 1 Then : N_mass = 1 : End If
'      Ch = N_mass : N_on = 5 : Gosub Out_out
      Q = N_mass : Gosub Choose_rev
      Mid(e_mass(n_prog) , N_step , 1) = Str_temp
'      N_mass = 1
   Elseif Flag_set = 3 Then
      If N_step <> 1 Then : Mid(e_mode(n_prog) , N_step , 1) = "2" : End If
   End If

Return
'###############################################################################






'###############################################################################
M_all:
Stop Timer1 : Stop Timer0
Enable Int4 : Enable Int5 : Enable Int6 : Enable Int7

If Flag = 0 Then                                            'если шаг закончил выполняться, то надо загрузить данные слеующего шага или программы
   If Led_on(n_prog) = 0 Then : Goto Label1 : End If
   Incr N_step

   If N_step = N_step_lim Then                              'если программа все-таки дошла до конца и не встретила End
Label3:
       Decr N_repeat_copy

       If N_repeat_copy = 0 Then
         N_repeat_copy = N_repeat
         N_step = 1
         Goto Label1
       End If

       N_step = 1
       Goto Label_end
Label1:
       Incr N_prog
       If N_prog = 17 Then
         If N_mode = 2 Then : N_prog = 1 : N_step = 0 : Goto Labl5 : Else : N_prog = 1 : End If
       End If
       If Led_on(n_prog) = 0 Then : Goto Label1 : End If
Label_end:
   End If


   'READING STRING//////////////////////////////////////////////////////////////
   Disable Interrupts
      Str_temp = Mid(e_mode(n_prog) , N_step , 1)           'чтенье режима работы
      If Str_temp = "0" Then
         N_mode = 0 : Flag_strike_pause = 0
      Elseif Str_temp = "1" Then
         N_mode = 1 : Flag_strike_pause = 1
      Elseif Str_temp = "2" Then
         If N_step = 1 Then : N_mode = 2 : Goto Label1
         Else : Goto Label3 : End If
      End If

      Str_temp = Mid(e_time(n_prog) , N_step , 1)           'чтенье времени паузы\удара
      Gosub Choose
      If N_mode = 1 Then
         Count1 = Lookup(r , Mass1 )
         Strike = Count1
         N_time = Count1 : N_time_f = 1
      Else
         Count1 = Lookup(r , Mass2 )
         Pause = Count1
         If Count1 > 99 Then                                'вывод времени на экран  (с запятой или без)
            N_time = Count1 / 10 : N_time_f = 0
         Else
            N_time = Count1 : N_time_f = 1
         End If
      End If


      If N_mode = 1 Then
      Str_temp = Mid(e_mass(n_prog) , N_step , 1)           'чтенье канала удара
      Gosub Choose
   '   N_mass = X + 1
      Ch = R
      End If
   Enable Interrupts
   'READING STRING//////////////////////////////////////////////////////////////
   'на выходе: N_mode, n_time, n_mass


   Flag = 1                                                 'установка флага переопределения на 1(false)

End If
Start Timer0 : Start Timer1
Labl5:
Return
'###############################################################################






'=================================================================
Adc_read:
   Y = Getadc(7) : Y = Y / 64 : X = Y                       'strike
   Adc_r(1) = Lookup(x , Mass1)

   Y = Getadc(6) : Y = Y / 64 : Q = Y
   If X > Q Then : Q = X : End If                           'если ручка MAX < MIN, то MAX=MIN
   Adc_r(2) = Lookup(q , Mass1)

   Y = Getadc(5) : Y = Y / 64 : X = Y                       'stop
   Adc_r(3) = Lookup(x , Mass2)

   Y = Getadc(4) : Y = Y / 64 : Q = Y
   If X > Q Then : Q = X : End If                           'если ручка MAX < MIN, то MAX=MIN
   Adc_r(4) = Lookup(q , Mass2)

Gosub Op1
Return
'=================================================================
Op1:
For X = 1 To 4
   If Adc_r(x) > 99 Then
      Adc_d(x) = Adc_r(x) / 10
      Adc_d_f(x) = 1
   Else
      Adc_d(x) = Adc_r(x)
      Adc_d_f(x) = 0
   End If
Next X
Return
'=================================================================
Mass1:                                                      'массив(х 0,1 сек) ударов  '255 макс = 25,5секунд
Data 1 , 2 , 3 , 5 , 6 , 7 , 8 , 10 , 12 , 15 , 18 , 20 , 30 , 40 , 50 , 100
Mass2:                                                      'массив(х 0,1 сек) пауз    '255 макс = 25,5секунд
Data 2 , 5 , 7 , 8 , 10 , 12 , 15 , 20 , 25 , 30 , 40 , 50 , 100 , 150 , 200 , 250
'///////////////////////

$include "display.bas"
$include "buttons.bas"

Choose_rev:
   Select Case Q
      Case 0 : Str_temp = "0"
      Case 1 : Str_temp = "1"
      Case 2 : Str_temp = "2"
      Case 3 : Str_temp = "3"
      Case 4 : Str_temp = "4"
      Case 5 : Str_temp = "5"
      Case 6 : Str_temp = "6"
      Case 7 : Str_temp = "7"
      Case 8 : Str_temp = "8"
      Case 9 : Str_temp = "9"
      Case 10 : Str_temp = "A"
      Case 11 : Str_temp = "B"
      Case 12 : Str_temp = "C"
      Case 13 : Str_temp = "D"
      Case 14 : Str_temp = "E"
      Case 15 : Str_temp = "F"
      Case 16 : Str_temp = "G"
      Case Else : Str_temp = "0"
   End Select
Return

Choose:
   Select Case Str_temp
      Case "0" : R = 0
      Case "1" : R = 1
      Case "2" : R = 2
      Case "3" : R = 3
      Case "4" : R = 4
      Case "5" : R = 5
      Case "6" : R = 6
      Case "7" : R = 7
      Case "8" : R = 8
      Case "9" : R = 9
      Case "A" : R = 10
      Case "B" : R = 11
      Case "C" : R = 12
      Case "D" : R = 13
      Case "E" : R = 14
      Case "F" : R = 15
      Case "G" : R = 16
      Case Else : R = 19
   End Select
Return


Init_out:                                                   'инициализация регистров, на все каналы запись 0
   Shiftout Portc.7 , Portc.6 , Aaa , 1
   Shiftout Portc.5 , Portc.4 , Aaa , 1
   Shiftout Portc.3 , Portc.2 , Aaa , 1
   Shiftout Portc.1 , Portc.0 , Aaa , 1
      Shiftout Porta.6 , Porta.7 , Bbb , 1
      Shiftout Porta.4 , Porta.5 , Bbb , 1
      Shiftout Porta.2 , Porta.3 , Bbb , 1
      Shiftout Porta.0 , Porta.1 , Bbb , 1
Pulseout Portg , 2 , 5
Return


Randomgen:                                                  'выбираем случайный канал из доступных
' W = 0 : Q = Rnd(8) : W = W + Q : Q = Rnd(4) : W = W + Q : Q = Rnd(2) : W = W + Q : Q = Rnd(1) : W = W + Q : Q = Rnd(1) : W = W + Q :
S1:
Gosub Checkmass
If Z = 1 Then
   Ch = Q
   Goto End_rand
End If

'Ch = Rnd(33) : Ch = Ch / 2
'      If Led_on(ch) = 0 Or Ch = 0 Then : Goto S1 : End If
'Y = Getadc(2)
'Q = Y / 15 : X = Rnd(16) : Ch = Q And X
Y = Rnd(timer1)
If Y < 16482 Or Y > 49446 Then : Goto S1 : End If
Y = Y Mod 10000
'Y = Y - 16482
Y = Y / 60
Ch = Y

'   W = Y / 100 : Q = Y Mod 10 : Q = Q Mod 10 : X = Y Mod 100
'   Ch = W - Q : Ch = Ch + X
   If Ch > 16 Or Ch = 0 Or Led_on(ch) = 0 Then : Goto S1 : End If       ' Or Ch = Last
'   Last = Ch
End_rand:
Return


Backup:
Gosub Init_out
Gosub Checkmass
If Z = 0 Then : Led_on(1) = 1 : End If
Enable Int4 : Enable Int5 : Enable Int6 : Enable Int7
Return