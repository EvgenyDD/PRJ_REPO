$regfile = "m48def.dat"                                     'определяем контроллер
$crystal = 8000000                                          'внутренний генератор
'$lib "mcsbyte.lbx"                                          'подключаем библиотеку функций

Ddrb = &B00000000
Ddrc = &B0111000                                            'остальная связь с миром
Ddrd = &B11111111                                           'драйвер семисегментов
Portb = &B00011110
Portc = &B1000000
Portd = &B00000000

A Alias Portd.7                                             'настройки подключения LED
B Alias Portd.5                                             '
C Alias Portd.3                                             '
D Alias Portd.1                                             '
E Alias Portd.0                                             '
F Alias Portd.6                                             '
G Alias Portd.4                                             '
Point Alias Portd.2                                         '

Da Alias Portc.3                                            'выводы дешифратора
Db Alias Portc.4                                            'выводы дешифратора
Dc Alias Portc.5                                            'выводы дешифратора

Water Alias Pinb.4                                          'датчик переполнения бака
'===============================================================================
Dim W As Byte                                               'универсальная константа
Dim Q As Byte                                               'универсальная константа
Dim N As Integer                                            'число срабатываний таймера === константа времени

Dim D_work_h As Byte                                        'красный дисплей
Dim D_work_l As Byte                                        'красный дисплей
Dim D_waiting_h As Byte                                     'зелёный дисплей
Dim D_waiting_l As Byte                                     'зелёный дисплей
'-------------------------------------------------------------------------------
'Config Watchdog = 2048                                      'проверка на зависание каждые 2 секунды
'Start Watchdog
Config Timer0 = Timer , Prescale = 8                        'настраиваем таймер
On Timer0 Int_0                                             'работа по таймеру 0
Enable Interrupts
Enable Timer0                                               'разрешаем работу таймера 0

'Gosub Eeprom                                                'все читаем из EEPROM
D_work_l = 20
D_work_h = 1
D_waiting_l = 10
D_waiting_h = 1

'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
Const T_displ = 2                                           'время(ms) переход на next цифру
Const H = &B01111001                                        'число H при переполнении воды
'Const X = 20                                                'debounce (ms)
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
Do
S:
If Water = 0 Then                                           'если сработал датчик воды
Portd = H                                                   'на индикаторе HHHHHH

For Q = 0 To 5
Gosub Displ_main
Next Q
Goto S
End If


Gosub Displ_out
Loop
End
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№№
'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
Int_0:                                                      'interrupts и включения/выключения насоса
Incr N

If N > 3906 Then                                            'константа времени - 1сек [3906.25]
Incr D_work_l
Incr D_waiting_l
N = 0
End If

Return
'###############################################################################
'###############################################################################
'###############################################################################
'###############################################################################
'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
Displ_out:                                                  'Вывод данных на дисплей
W = D_work_h                                                'берём единицы минут
Portd = Lookup(w , Segments)
Set Point
Q = 0
Gosub Displ_main

W = D_work_l / 10                                           'берём десятки секунд
Portd = Lookup(w , Segments)
Q = 1
Gosub Displ_main

W = D_work_l Mod 10                                         'берём последнюю {справа} цифру числа
Portd = Lookup(w , Segments)
Q = 2
Gosub Displ_main
'=====
W = D_waiting_h                                             'берём единицы минут
Portd = Lookup(w , Segments)
Set Point
Q = 3
Gosub Displ_main

W = D_waiting_l / 10                                        'берём десятки секунд
Portd = Lookup(w , Segments)
Q = 4
Gosub Displ_main

W = D_waiting_l Mod 10                                      'берём последнюю {справа} цифру числа
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
