$regfile = "m48def.dat"                                     'определяем контроллер
$crystal = 8000000                                          'внутренний генератор
'$lib "mcsbyte.lbx"                                          'подключаем библиотеку функций

Ddrb = &B00000000
Ddrc = &B0111000                                            'остальная связь с миром
Ddrd = &B11111111                                           'драйвер семисегментов
Portb = &B00011110
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
Minus Alias Pinb.2                                          '
Modes Alias Pinb.3                                          '
Indicator Alias Portb.0                                     '
Rele Alias Portb.5                                          'выход на реле насоса
Water Alias Pinb.4                                          'датчик переполнения бака

'A F B G C H D E
'SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
'SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
Dim W As Byte                                               'универсальная константа
Dim Q As Byte                                               'универсальная константа

Dim M As Byte                                               'индикатор включения/выключения насоса
'--------------------дисплейные и датовые---------------------------------------
Dim N As Integer                                            'число срабатываний таймера === константа времени
Dim Seconds As Word                                         'прошедшие секунды

Dim Uph As Byte                                             'время на работу насоса реальное
Dim Upl As Byte                                             'время на работу насоса
Dim Dnh As Byte                                             'время на ожидание
Dim Dnl As Byte                                             'время на ожидание

Dim Duph As Byte                                            'красный дисплей на вывод
Dim Dupl As Byte                                            'красный дисплей
Dim Ddnh As Byte                                            'зелёный дисплей
Dim Ddnl As Byte                                            'зелёный дисплей
'-------------------------------------------------------------------------------
'Config Watchdog = 2048                                      'проверка на зависание каждые 2 секунды
'Start Watchdog
Config Timer0 = Timer , Prescale = 8                        'настраиваем таймер
On Timer0 Int_0                                             'работа по таймеру 0
Enable Interrupts
Enable Timer0                                               'разрешаем работу таймера 0

'Gosub Eeprom                                                'все читаем из EEPROM
Uph = 1
Upl = 50
Dnh = 1
Dnl = 20
M = 1
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
Const T_displ = 2                                           'время(ms) переход на next цифру
Const H_water = &B01111001                                  'число H при переполнении воды
Const Deb = 20                                              'debounce (ms)
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
For Q = 0 To 5
Gosub Displ_main
Next Q
Goto S
End If

'Reset Watchdog
Gosub Displ_n

Loop
End
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
Int_0:                                                      'interrupts и включения/выключения насоса
Incr N

If N > 1800 Then                                            'константа времени - 1сек [3906.25]
Incr Seconds
N = 0
End If

Return
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
'###############################################################################
'###############################################################################
'###############################################################################
'###############################################################################
Displ_n:                                                    'проверка(нормальная работа)
If M = 0 Then                                               'если насос выключен

  If Ddnl = 0 And Ddnh = 0 Then
       M = 1
       Rele = 1
       Goto K
  End If

If Ddnl = 0 Or Ddnl = 255 Then
     Ddnl = 59
End If

 Dupl = Upl
 Duph = Uph
 W = Seconds Mod 60
 Q = Seconds / 60
 Ddnl = Dnl - W                                             'данные выводятся в виде таймера
 Ddnh = Dnh - Q

 Indicator = 0
End If
'===============================================================================
If M = 1 Then                                               'если насос включен

  If Dupl = 0 And Duph = 0 Then
       M = 0
       Rele = 0
       Goto K
  End If

If Dupl = 0 Or Dupl = 255 Then
     Dupl = 59

End If

 W = Seconds Mod 60
 Q = Seconds / 60
 Dupl = Upl - W
 Duph = Uph - Q
 Ddnl = Dnl                                                 'данные выводятся в виде таймера
 Ddnh = Dnh

 Indicator = 1
End If

 Goto L
K:                                                          'смена режима - обнуление и default
 Seconds = 0
 Dupl = Upl
 Duph = Uph
 Ddnl = Dnl
 Ddnh = Dnh
L:
Gosub Displ_out
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
'###############################################################################
Segments:                                                   'Матрица для вывода на LED
Data &B11101011 , &B00101000 , &B10110011 , &B10111010 , &B01111000 , &B11011010 , &B11011011 , &B10101000 , &B11111011 , &B111111010
Positions:                                                  'Матрица для вывода на LED
Data &B0010000 , &B0001000 , &B0110000 , &B0011000 , &B0100000 , &B0101000
