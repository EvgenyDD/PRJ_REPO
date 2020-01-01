$regfile = "m48def.dat"                                     'определяем контроллер
$crystal = 8000000                                          'внутренний генератор
'$lib "mcsbyte.lbx"                                          'подключаем библиотеку функций

Ddrb = &B00000000
Ddrc = &B0111101                                            'остальная связь с миром
Ddrd = &B11111111                                           'драйвер семисегментов
Portb = &B11010010
Portc = &B1000000
Portd = &B00000000

'A Alias Portd.7                                             'настройки подключения LED
'B Alias Portd.5                                             '
'C Alias Portd.3                                             '
'D Alias Portd.1                                             '
'E Alias Portd.0                                             '
'F Alias Portd.6                                             '
'G Alias Portd.4                                             '
Point Alias Portd.2

'Da Alias Portc.3                                            'выводы дешифратора
'Db Alias Portc.4                                            'выводы дешифратора
'Dc Alias Portc.5                                            'выводы дешифратора

Plus Alias Pinb.1                                           '
Minus Alias Pinb.7                                          '
Modes Alias Pinb.6                                          '
Off_led Alias Portb.0                                       '
On_led Alias Portc.2
Rele Alias Portc.0                                          'выход на реле насоса
Water Alias Pinb.4                                          'датчик переполнения бака

'A F B G C H D E
'SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
'SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
Dim W As Byte                                               'универсальная константа
Dim Q As Byte                                               'универсальная константа

Dim M As Byte                                               'индикатор включения/выключения насоса
Dim Position As Byte                                        'позиция курсора при установке времени
Dim B As Bit

'--------------------дисплейные и датовые---------------------------------------
Dim N As Integer                                            'число срабатываний таймера === константа времени

Dim Uph As Byte                                             'время на работу насоса реальное
Dim Upl As Byte                                             'время на работу насоса
Dim Dnh As Byte                                             'время на ожидание
Dim Dnl As Byte                                             'время на ожидание

Dim Duph As Byte                                            'красный дисплей на вывод
Dim Dupl As Byte                                            'красный дисплей
Dim Ddnh As Byte                                            'зелёный дисплей
Dim Ddnl As Byte                                            'зелёный дисплей

'===============================================================================
Dim Uph_e As Eram Byte At 1                                 'время на работу насоса eeprom
Dim Upl_e As Eram Byte At 2                                 'время на работу насоса eeprom
Dim Dnh_e As Eram Byte At 3                                 'время на ожидание eeprom
Dim Dnl_e As Eram Byte At 4                                 'время на ожидание eeprom
'===============================================================================
'Config Watchdog = 2048                                      'проверка на зависание каждые 2 секунды
'Start Watchdog
Config Timer0 = Timer , Prescale = 8                        'настраиваем таймер
On Timer0 Int_0                                             'работа по таймеру 0
Enable Interrupts
Enable Timer0                                               'разрешаем работу таймера 0

'Gosub Eeprom                                                'все читаем из EEPROM
Uph = Uph_e                                                 'все читаем из EEPROM
Upl = Upl_e
Dnh = Dnh_e
Dnl = Dnl_e
M = 0
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
Const T_displ = 2                                           'время(ms) переход на next цифру
Const H_water = &B01111001                                  'число H при переполнении воды
Const Deb = 200                                             'debounce (ms)
Const Time_n = 4086                                         'константа времени - 1сек [4086]
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
 Dupl = Upl
 Duph = Uph
 Ddnl = Dnl
 Ddnh = Dnh
Do
S:
If Water = 0 Then                                           'если сработал датчик воды
Portd = H_water                                             'на индикаторе HHHHHH
  Rele = 0
  Stop Timer0
 For Q = 0 To 5
  Gosub Displ_main
  Off_led = 1
  On_led = 1
 Next Q
   If Water = 1 Then
      Off_led = 0
      On_led = 0
      Start Timer0
   End If
Goto S
End If

If Modes = 0 Then

 Dupl = Upl
 Duph = Uph
 Ddnl = Dnl
 Ddnh = Dnh
  Incr Position
  Stop Timer0
  Disable Interrupts
  Gosub Miganie
  Gosub Change
Start Timer0
Enable Interrupts
End If

 If Plus = 0 And M = 0 Then                                 'при нажатии кнопки (+) происходит включение насоса
   M = 1
   Rele = 1
 Dupl = Upl
 Duph = Uph
 Ddnl = Dnl
 Ddnh = Dnh
   Waitms Deb
  End If

  If Minus = 0 And M = 1 Then                               'при нажатии кнопки (-) происходит выключение насоса
   M = 0
   Rele = 0
 Dupl = Upl
 Duph = Uph
 Ddnl = Dnl
 Ddnh = Dnh
   Waitms Deb
  End If

'Reset Watchdog
Gosub Displ_out

Loop
End
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
Int_0:                                                      'interrupts и включения/выключения насоса
Incr N
'===============================================================================
If M = 0 And N > Time_n Then
N = 0
 Decr Ddnl
 Off_led = 1
 On_led = 0

   If Ddnl = 0 And Ddnh = 0 Then
       M = 1
       Rele = 1

       Goto K
   End If

 If Ddnl = 255 Then
  Decr Ddnh
  Ddnl = 59
 End If
End If
'&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
If M = 1 And N > Time_n Then
N = 0
 Decr Dupl
 Off_led = 0
 On_led = 1

   If Dupl = 0 And Duph = 0 Then
       M = 0
       Rele = 0

       Goto K
   End If

 If Dupl = 255 Then
  Decr Duph
  Dupl = 59
 End If
End If
'&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 Goto L
K:                                                          'смена режима - обнуление и default
 Dupl = Upl
 Duph = Uph
 Ddnl = Dnl
 Ddnh = Dnh
L:
Return
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
Change:
While Position > 0

If Modes = 0 Then
  Off_led = 0
  On_led = 0
  Waitms 250
  Off_led = 1
  On_led = 1
  Waitms 100
     Incr Position
  Off_led = 1
  On_led = 1
   'Waitms Deb
     If Position = 7 Then                                   'конец установкам
         Position = 0
         Upl = Dupl
         Uph = Duph
         Dnl = Ddnl
         Dnh = Ddnh
         Uph_e = Uph                                        'все записываем в EEPROM
         Upl_e = Upl
         Dnl_e = Dnl
         Dnh_e = Dnh

        Gosub Miganie
     End If
End If

Stop Timer0
Reset Rele
'%%%%%%%%%%%%%%%%%%%%%%%%%
  If Plus = 0 Then                                          'при нажатии кнопки (+) происходит увеличение числа
  B = 1
   Gosub Up_down
   Waitms Deb
  End If

  If Minus = 0 Then                                         'при нажатии кнопки (-) происходит уменьшение числа
  B = 0
   Gosub Up_down
   Waitms Deb
  End If
'%%%%%%%%%%%%%%%%%%%%%%%%%
Gosub Displ_out
Wend
Return
'###############################################################################
'###############################################################################
'###############################################################################
'###############################################################################
Up_down:                                                    'изменение данных при нажатии на кнопки с учетом границ чисел

On Position Goto End_on , C_1 , C_2 , C_3 , C_4 , C_5 , C_6 'выбор изменяемой цифры
'End If

C_1:
W = Duph                                                    '+/- минут
Gosub T1
Duph = W
Goto End_on

C_2:
Q = 10                                                      'при нажатии (+) единицы секунд +/- на Q
W = Dupl
Gosub T2
Dupl = W
Goto End_on

C_3:
Q = 1                                                       'при нажатии (+) единицы секунд +/- на Q
W = Dupl
Gosub T2
Dupl = W
Goto End_on

C_4:
W = Ddnh                                                    '+/- минут
Gosub T1
Ddnh = W
Goto End_on

C_5:
Q = 10                                                      'при нажатии (+) единицы секунд +/- на Q
W = Ddnl
Gosub T2
Ddnl = W
Goto End_on

C_6:
Q = 1                                                       'при нажатии (+) единицы секунд +/- на Q
W = Ddnl
Gosub T2
Ddnl = W
Goto End_on
'8888888888888888888888888888888888888888888888888888888888888888888888888888888
T1:                                                         'увеличение оставшегося места на чипе
If B = 1 Then
Incr W
Else
Decr W
End If
'==========
If W > 9 Or W < 0 Then
W = 0
End If
Return

T2:                                                         'увеличение оставшегося места на чипе
If B = 1 Then
W = W + Q
Else
W = W - Q
End If
'==========
If W < 0 Or W > 59 Then
W = 0
End If
Return

End_on:
Return
'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
Displ_out:                                                  'Вывод данных на дисплей
W = Duph                                                    'берём единицы минут
Portd = Lookup(w , Segments)
Set Point
Q = 0
Gosub Displ_main

W = Dupl / 10                                               'берём десятки секунд
Portd = Lookup(w , Segments)
Q = 1
Gosub Displ_main

W = Dupl Mod 10                                             'берём последнюю {справа} цифру числа
Portd = Lookup(w , Segments)
Q = 2
Gosub Displ_main
'=====
W = Ddnh                                                    'берём единицы минут
Portd = Lookup(w , Segments)
Set Point
Q = 3
Gosub Displ_main

W = Ddnl / 10                                               'берём десятки секунд
Portd = Lookup(w , Segments)
Q = 4
Gosub Displ_main

W = Ddnl Mod 10                                             'берём последнюю {справа} цифру числа
Portd = Lookup(w , Segments)
Q = 5
Gosub Displ_main
Return
'------------------
Displ_main:                                                 'берём q - номер индикатора(0-5)+ждём T_displ
 W = Lookup(q , Positions)
 Portc = W Or Pinc
 Waitms T_displ
 Reset Portc.3
 Reset Portc.4
 Reset Portc.5

Return
'###############################################################################
'#########################' 'A F B G C H D E '##################################
'###############################################################################
Miganie:
For W = 0 To 3
  Off_led = 0
  On_led = 0
  Waitms 100
  Off_led = 1
  On_led = 1
  Waitms 110
Next
Return
'###############################################################################
Segments:                                                   'Матрица для вывода на LED
Data &B11101011 , &B00101000 , &B10110011 , &B10111010 , &B01111000 , &B11011010 , &B11011011 , &B10101000 , &B11111011 , &B111111010
Positions:                                                  'Матрица для вывода на LED
Data &B0010000 , &B0001000 , &B0110000 , &B0011000 , &B0100000 , &B0101000