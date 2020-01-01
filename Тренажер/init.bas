Ddra = &B11111111
Ddrb = &B11100111
Ddrc = &B11111111
Ddrd = &B11100000
Ddre = &B00000000
Ddrf = &B00000101
Ddrg = &B11100

Porta = &B00000000
Portb = &B00000111
Portc = &B00000000
Portd = &B00001000
Porte = &B00000011
Portf = &B00001010
Portg = &B00011

'|||||||||||\\\-----
Config Adc = Single , Prescaler = Auto , Reference = Avcc
Config Timer0 = Timer , Prescale = 8                        'настраиваем таймер 0
Config Timer1 = Timer , Prescale = 1
'Config Timer2 = Timer , Prescale = 8
On Timer0 Int_0                                             'работа по таймеру 0
Enable Timer0                                               'разрешаем работу таймера 0
Stop Timer0
Stop Timer1
'Start Timer2

'Enable Timer0
Config Int4 = Rising
Config Int5 = Rising
Config Int6 = Rising
Config Int7 = Rising

Config Int0 = Rising

'Config Int3 = Falling
'On Int3 Button_21                                           'Nosave

On Int4 Button_1                                            'Nosave                                     ' Nosave
On Int5 Button_2                                            ' Nosave
On Int6 Button_3                                            ' Nosave
On Int7 Button_4                                            ' Nosave
On Int0 Button_5                                            ' Nosave
Enable Int4
Enable Int5
Enable Int6
Enable Int7
Enable Int0
'Enable Int3
Enable Interrupts
'|||||||||||///-----

Col1 Alias Portf.3
Col2 Alias Portf.1
Col3 Alias Porte.0
Col4 Alias Porte.1

Cold1 Alias Ddrf.3
Cold2 Alias Ddrf.1
Cold3 Alias Ddre.0
Cold4 Alias Ddre.1

Row1 Alias Pine.4
Row2 Alias Pine.5
Row3 Alias Pine.6
Row4 Alias Pine.7
Row5 Alias Pind.0

Rowd1 Alias Ddre.4
Rowd2 Alias Ddre.5
Rowd3 Alias Ddre.6
Rowd4 Alias Ddre.7
Rowd5 Alias Ddrd.0