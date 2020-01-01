$regfile = "m8def.dat"
$crystal = 8000000
$lib "mcsbyte.lbx"


Ddrb = &B11111111
Ddrc = &B0110000
Ddrd = &B00100001

Portb = &B00000001
Portc = &B1000000
Portd = &B00011110
'-------------------------------------------------------------------------------
Const Disp_time = 500                                       'мкс
'-------------------------------------------------------------------------------
Key Alias Portb.0                                           'выходное реле
Rele Alias Portd.5                                          'реле переключения напряжения (10 или 30 вольт)
U_pwm Alias Pwm1b                                           'ШИМ напряжения
I_pwm Alias Pwm1a                                           'ШИМ тока (для компаратора)
Fan_pwm Alias Ocr2                                          'Регулировка вентилятора
'-------------------------------------------------------------------------------
Dim Mode_n As Byte
Dim Disp_n As Byte
Dim Enc As Byte

Dim Disp As Byte                                            'цифра или символ для отправки на одну цифру
Dim Digit As Byte                                           'нужная цифра

Dim Temp As Single

Dim U_real As Word
Dim I_12 As Word                                            'значение тока в мА       канал 12 вольт      (2мА-1.999А)
Dim I_5 As Word                                             'значение тока в мА       канал 5 вольт       (2мА-1.999А)
Dim I_800 As Word                                           'значение тока в мА       регулируемый канал  (1мА-0.800А)
Dim I_99 As Word                                            'значение тока в 100мА    регулируемый канал  (0.10А-9.90А)

Dim Um_real As Single
Dim Im_12 As Single                                         'значение тока в мА       канал 12 вольт      (2мА-1.999А)
Dim Im_5 As Single                                          'значение тока в мА       канал 5 вольт       (2мА-1.999А)
Dim Im_800 As Single                                        'значение тока в мА       регулируемый канал  (1мА-0.800А)
Dim Im_99 As Single                                         'значение тока в 100мА    регулируемый канал  (0.10А-9.90А)

Dim I_string As Byte                                        'барграф тока (1-6)

Dim Btn As Bit                                              'синий LED в кнопке
Dim Overload As Bit                                         'красные леды перегруза

Dim Cycle As Word
Dim Cycle1 As Word
Dim X As Byte
Dim Y As Word
Dim Z As Word

$include "DISPLAY.bas"

Set Portb.0                                                 'включаем выход

Disp_n = 1                                                  'режим 5в
I_string = 3                                                'в барграф
Cycle = 0 : Cycle1 = 0
Rele = 0
'-------------------------------------------------------------------------------
Config Int0 = Falling : Config Int1 = Falling               'энкодер
Config Adc = Single , Prescaler = 128 , Reference = Aref    'ADC
Config Timer0 = Timer , Prescale = 64
Config Timer1 = Pwm , Pwm = 10 , Compare A Pwm = Clear Down , Compare B Pwm = Clear Down , Noise Cancel = 1 , Prescale = 1
Config Timer2 = Pwm , Prescale = 1024 , Pwm = On , Compare Pwm = Clear Up

On Timer0 Int_time
On Int0 Encoder_button
On Int1 Encoder0

Start Timer0 : Start Timer1 : Start Timer2
Start Adc
Enable Int0 : Enable Int1 : Enable Timer0
Enable Interrupts
Gosub Fan
 Overload = 0
 Btn = 0







'//////////////////////MAIN/////////////////////////////////////////////////////
'###############################################################################
'###############################################################################
Do
   If Pind.1 = 0 Then                                       'прерывание от кнопки с LED
 A:
   If Pind.1 = 0 Then : Goto A : End If
      Incr Disp_n : Waitms 50
      If Disp_n = 3 Then : Disp_n = 0 : End If              'limitting
   End If

   Gosub Measuring

   On Disp_n Gosub Display_main , Display_sub5 , Display_sub12       'тип индицируемых данных
   Gosub Display_2                                          'светодиодная шкала и остальный светодиоды

   Gosub Fan                                                'обработка температуры и включение вентилятора
   Gosub Button_check
   'Set Ddrd.1
   'Sound  Portd.1 , 100 , 500
   'Reset Ddrd.1

Loop
'###############################################################################
'###############################################################################










'//////////////////////INTs/////////////////////////////////////////////////////
Encoder0:                                                   'энкодер
   Waitus 500
   If Pind.4 = 0 Then : If U_pwm >= 4 Then : U_pwm = U_pwm - 4 : End If       'Enc = 1
   Else : If U_pwm <= 1019 Then : U_pwm = U_pwm + 4 : End If : End If       'Enc = 2
   Gifr = 128
Return
'*******************************************************************************
Encoder_button:                                             'кнопка энкодера
   Incr Mode_n                                              'изменение режима защиты и устанавливаемого параметра
   If Mode_n = 3 Then : Mode_n = 0 : End If
   Toggle Btn
Incr I_string
If I_string = 7 Then
   I_string = 1
End If
Toggle Overload
   Gifr = 64
Return
'*******************************************************************************
Int_time:                                                   'измерение тока и защита
   Waitus 200
Return








'//////////////////////SUBs/////////////////////////////////////////////////////
'0000000000000000000000000000000000000000000000000000000000000000000000000000000
Registr:                                                    'подпрограмма отправки данных в регистр
   Shiftout Portc.4 , Portd.0 , Disp , 1                    'задвигаем данные
   Pulseout Portc , 5 , 1                                   'защелкиваем регистр
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
Measuring:                                                  'измерение тока и защита
Incr Cycle1
If Cycle1 = 21 Then
   Um_real = Um_real * 0.2711                               'измерение напряжения
   Um_real = Um_real + 1.3205
   U_real = Um_real
'////////
   Im_5 = Im_5 * 2.1653                                     '2.16533153*x-50.92690724
   Im_5 = Im_5 - 50.9269
   If Im_5 < 1 Then : I_5 = 0 : Else : I_5 = Im_5 : End If
   '////////
   Im_12 = Im_12 * 2.1194                                   '2.11940657*x-34.55231898
   Im_12 = Im_12 - 34.5523
   If Im_12 < 1 Then : I_12 = 0 : Else : I_12 = Im_12 : End If
   '////////
   Im_800 = Im_800 * 0.9211                                 '0.9211870259*x-11.47819492
   Im_800 = Im_800 - 11.4781
   If Im_800 < 1 Then : I_800 = 0 : Else : I_800 = Im_800 : End If
   '////////
   Im_99 = Im_99 * 9.34214                                  '9.342140771*x-184.8688295
   Im_99 = Im_99 - 184.8688
   If Im_99 < 1 Then : I_99 = 0 : Else : I_99 = Im_99 : End If

   Cycle1 = 0
   Im_99 = 0 : Im_800 = 0 : Im_12 = 0 : Im_5 = 0 : Um_real = 0
Else
   Y = Getadc(7)
   Temp = Y / 20
   Um_real = Um_real + Temp

   Y = Getadc(1)
   Temp = Y / 20
   Im_5 = Im_5 + Temp

   Y = Getadc(2)
   Temp = Y / 20
   Im_12 = Im_12 + Temp

   Y = Getadc(0)
   Temp = Y / 20
   Im_800 = Im_800 + Temp

   Y = Getadc(6)
   Temp = Y / 20
   Im_99 = Im_99 + Temp

End If

Return
'0000000000000000000000000000000000000000000000000000000000000000000000000000000
Button_check:
Z = 0
   Shiftout Portc.4 , Portd.0 , Z , 1                       'задвигаем данные
   Pulseout Portc , 5 , 1                                   'защелкиваем регистр
Ddrb = &B00001111
Portb = &B11110001

If Pinb.4 = 0 Then : I_string = 2
End If

If Pinb.7 = 0 Then : I_string = 4
End If

Ddrb = &B11111111
Gosub Resetting
Return
'0000000000000000000000000000000000000000000000000000000000000000000000000000000
Fan:
Incr Cycle                                                  'обработка температуры и включение вентилятора
If Cycle = 1000 Then

   Y = Getadc(3)                                            'выход термосопротивления радиатора
   Z = Y
   Select Case Y
      Case 0 To 470 : Fan_pwm = 0
      Case 471 To 484 : Fan_pwm = 60
      Case 485 To 499 : Fan_pwm = 100
      Case 500 To 519 : Fan_pwm = 120
      Case 520 To 539 : Fan_pwm = 150
      Case 540 To 559 : Fan_pwm = 170
      Case 560 To 579 : Fan_pwm = 200
      Case 580 To 598 : Fan_pwm = 220
      Case 600 To 1023 : Fan_pwm = 255
   End Select

   Cycle = 0
End If
Return
'###############################################################################