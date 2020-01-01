$regfile = "m48def.dat"                                     'определяем контроллер
$crystal = 8000000                                          'внутренний генератор


Ddrb = &B00000001
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
Dim Z As Byte
Dim R As Word
Dim Y As Byte                                               'индикатор  мигания
'--------------------дисплейные и датовые---------------------------------------
Dim N As Integer                                            'число срабатываний таймера === константа времени
Dim N1 As Word
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
Config Watchdog = 2048                                      'проверка на зависание каждые 2 секунды
Start Watchdog
Config Timer0 = Timer , Prescale = 8                        'настраиваем таймер 0
On Timer0 Int_0                                             'работа по таймеру 0
Enable Interrupts
Enable Timer0                                               'разрешаем работу таймера 0
Config Timer1 = Timer , Prescale = 8                        'настраиваем таймер 1
On Timer1 Int_1                                             'работа по таймеру 0
Enable Timer1
Stop Timer1

Uph = Uph_e                                                 'все читаем из EEPROM
Upl = Upl_e
Dnh = Dnh_e
Dnl = Dnl_e
Uph = 5                                                     'для начальной установки
Upl = 10                                                    'для начальной установки
Dnh = 2                                                     'для начальной установки
Dnl = 30                                                    'для начальной установки
M = 0
R = 20
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
Const T_displ = 2                                           'время(ms) переход на next цифру
Const H_water = &B01111001                                  'число H при переполнении воды
Const Deb = 200                                             'debounce (ms)
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
  Reset Watchdog
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
Stop Timer0
   Rele = 0
Wait 0.9
Reset Watchdog
 If Modes = 0 Then
   Goto Count

 Else
Wait 1
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
End If
Fg:

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

Reset Watchdog
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
If M = 0 And N > 4086 Then                                  'константа времени - 1сек [4086]
N = 0
 Decr Ddnl
Fq2:
 Off_led = 1
 On_led = 0
       M = 0
       Rele = 0
   If Ddnl = 0 And Ddnh = 0 Then
       M = 1
       Rele = 1

       Ddnl = Dnl
       Ddnh = Dnh
       Goto Fq1
   End If

 If Ddnl = 255 Then
  Decr Ddnh
  Ddnl = 59
 End If

End If
'&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
If M = 1 And N > 4086 Then                                  'константа времени - 1сек [4086]
N = 0
 Decr Dupl
Fq1:
 Off_led = 0
 On_led = 1
       M = 1
       Rele = 1
   If Dupl = 0 And Duph = 0 Then
       M = 0
       Rele = 0

       Dupl = Upl
       Duph = Uph
       Goto Fq2
   End If

 If Dupl = 255 Then
  Decr Duph
  Dupl = 59
 End If

End If
'&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 Goto L
K:                                                          'смена режима - обнуление и default


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
T1:
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

T2:
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
Reset Watchdog
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
If Position > 0 Then
  Gosub Flash                                               'переходим если режим настройки
Else
X:
 W = Lookup(q , Positions)                                  'если обычный режим
 Portc = W Or Pinc
 Waitms T_displ
 Reset Portc.3
 Reset Portc.4
 Reset Portc.5
 Reset Watchdog
End If
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
  Reset Watchdog
Next
Return
'###############################################################################
Segments:                                                   'Матрица для вывода на LED
Data &B11101011 , &B00101000 , &B10110011 , &B10111010 , &B01111000 , &B11011010 , &B11011011 , &B10101000 , &B11111011 , &B111111010
Positions:                                                  'Матрица для вывода на LED
Data &B0010000 , &B0001000 , &B0110000 , &B0011000 , &B0100000 , &B0101000




Flash:
W = Position - 1
If Q = W Then                                               'переходим если данная цифра - устанавливаемая
 On Y Gosub Mode1 , Mode2
Else
Goto X
End If
Reset Watchdog                                              'переходим если данная цифра - не устанавливаемая
Return

'===============================================================================
Mode1:
Decr R
   If R = 0 Then
     W = Lookup(q , Positions)
     Portc = W Or Pinc
     Waitms T_displ
     Reset Portc.3
     Reset Portc.4
     Reset Portc.5
     Reset Watchdog
     Y = 1
   End If
Return
'===============================================================================
Mode2:
Incr R
   If R <> 20 Then
     W = Lookup(q , Positions)
     Portc = W Or Pinc
     Waitms T_displ
     Reset Portc.3
     Reset Portc.4
     Reset Portc.5
     Reset Watchdog
   Else
     Y = 0
   End If
Return
'===============================================================================
Int_1:
Incr N1
If B = 0 Then
'===============================================================================
Rele = 1
Reset Watchdog
 If N1 > 16 Then                                            'константа времени - 1сек [32]
  Incr Dupl
   If Dupl = 60 Then
   Dupl = 0
   Incr Duph
   End If
   If Dupl = 59 And Duph = 9 Then
   Goto I
   End If
  Toggle On_led
  Off_led = 1
  N1 = 0
 End If
Else
Rele = 0
'===============================================================================
Reset Watchdog
 If N1 > 16 Then                                            'константа времени - 1сек [32]
  Incr Ddnl
   If Ddnl = 60 Then
   Ddnl = 0
   Incr Ddnh
   End If
   If Ddnl = 59 And Ddnh = 9 Then
   Goto I
   End If
  Toggle Off_led
  On_led = 1
  N1 = 0
 End If

End If
Return
'###############################################################################
Count:
Stop Timer0
 Dupl = 0
 Duph = 0
 Ddnl = 0
 Ddnh = 0
 Toggle On_led
 Toggle Off_led
 Waitms 150
 Toggle On_led
 Toggle Off_led
 Waitms 150
 Toggle On_led
 Toggle Off_led
 Waitms 150
 Toggle On_led
 Toggle Off_led
 Waitms 150
Position = 8
Reset Watchdog
Wait 1
'===============================================================================
While Position = 8

If Plus = 1 Then
 If Modes = 0 Then
  Position = 0
  Goto I
 End If
 Toggle On_led
 Toggle Off_led
 Waitms 150
 Reset Watchdog
Else
 Goto Up_wait
 Reset Watchdog
End If

If Minus = 1 Then
 If Modes = 0 Then
  Position = 0
  Goto I
 End If
 Toggle On_led
 Toggle Off_led
 Waitms 150
 Reset Watchdog
Else
 Goto Dn_wait
 Reset Watchdog
End If

Wend
'===============================================================================
I:
Stop Timer1
Wait 1
If Duph <> 0 Or Dupl <> 0 And Ddnl <> 0 Or Ddnh <> 0 Then
         Upl = Dupl
         Uph = Duph
         Dnl = Ddnl
         Dnh = Ddnh
         Uph_e = Uph                                        'все записываем в EEPROM
         Upl_e = Upl
         Dnl_e = Dnl
         Dnh_e = Dnh
End If
N1 = 0
Rele = 0
Z = 0
Start Timer0
 Dupl = Upl
 Duph = Uph
 Ddnl = Dnl
 Ddnh = Dnh
 Reset Watchdog
Goto Fg

'-------------------------------------------------------------------------------
Up_wait:
Reset Watchdog
B = 0
Start Timer1
If Minus = 0 Then

 If Z = 0 Then
  Z = 1
  Goto Dn_wait
 End If
 Goto Up_wait

Else

   If Modes = 0 Then
     Position = 0
     Goto I
   End If
 Gosub Displ_out
 Goto Up_wait

End If

'-------------------------------------------------------------------------------
Dn_wait:
Reset Watchdog
B = 1
Start Timer1
If Plus = 0 Then

 If Z = 0 Then
  Z = 1
  Goto Up_wait
 End If
 Goto Dn_wait

Else

   If Modes = 0 Then
     Position = 0
     Goto I
   End If
 Gosub Displ_out
 Goto Dn_wait

End If

'-------------------------------------------------------------------------------