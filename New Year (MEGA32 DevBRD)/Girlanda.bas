$regfile = "m32def.dat"
$crystal = 20000000

Ddra = &B11111111
Ddrb = &B11111111
Ddrc = &B11111111
Ddrd = &B11111111
Porta = &B00001111
Portb = &B00000000
Portc = &B00000000
Portd = &B00000000
'===============================================================================
Dim X As Byte
Dim Y As Byte

Dim Rgb_1 As Integer
Dim Rgb_2 As Integer
Dim Rgb_3 As Integer
Dim Rgb_4 As Integer

Dim Tik As Byte

A Alias Porta.0                                             'red
B Alias Porta.2                                             'grn
C Alias Porta.3                                             'blu
D Alias Porta.1                                             'yel
'========================Init===================================================
Config Timer0 = Timer , Prescale = 1                        '����������� ������
Enable Timer0                                               '��������� ������ ������� 0
Enable Interrupts                                           '��������� ����������
On Timer0 Pwm_out                                           '������ �� ������� 0
Start Timer0                                                '����� ������� 0
'===============================================================================
'===========================main cycle  ========================================
Do
Stop Timer0

For X = 1 To 10                                             '�������
Gosub M1
Next X

For X = 1 To 15                                             '������� �����
Gosub M2
Next X

For X = 1 To 20                                             '������� ���� 2 ����
Gosub M3
Next X

Start Timer0
For Y = 1 To 200                                            '������� ������� ������� PWM
Gosub M4
Next Y

For Y = 1 To 60                                             '������� ������� ��������� RELAX  PWM
Gosub M5
Next Y

For Y = 1 To 60                                             '2-2 flashing plavnoe PWM
Gosub M6
Next Y

For Y = 1 To 60                                             '������� ����� ��������� PWM
Gosub M7
Next Y
Stop Timer0

Loop
End
'%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
M1:
   A = 0
   B = 0
   C = 0
   D = 0
   Waitms 800
   A = 1
   B = 1
   C = 1
   D = 1
   Waitms 800
Return
'%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
M2:
Const T_1 = 400
   A = 0                                                    '���.�����
   B = 0
   C = 0
   D = 0
   Waitms T_1
   A = 0
   B = 0
   C = 0
   D = 1
   Waitms T_1
   A = 0
   B = 0
   C = 1
   D = 1
   Waitms T_1
   A = 0
   B = 1
   C = 1
   D = 1
   Waitms T_1
   A = 1
   B = 1
   C = 1
   D = 1
   Waitms T_1
   A = 1
   B = 1
   C = 1
   D = 0
   Waitms T_1
   A = 1
   B = 1
   C = 0
   D = 0
   Waitms T_1
   A = 1
   B = 0
   C = 0
   D = 0
   Waitms T_1
Return
'$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
M3:
Const T_2 = 400
   A = 1
   B = 1
   C = 0
   D = 0
   Waitms T_2
   A = 0
   B = 1
   C = 1
   D = 0
   Waitms T_2
   A = 0
   B = 0
   C = 1
   D = 1
   Waitms T_2
   A = 1
   B = 0
   C = 0
   D = 1
   Waitms T_2
Return
'%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
M4:                                                         '�������  - ���������
For X = 1 To 255
Rgb_1 = X
Rgb_2 = X
Rgb_3 = X
Rgb_4 = X
Waitus 100                                                  '�������
Next X

For X = 255 To 1 Step -1
Rgb_1 = X
Rgb_2 = X
Rgb_3 = X
Rgb_4 = X
Waitus 100                                                  '�������
Next X

Return
'%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
M5:
   For X = 1 To 255
Rgb_1 = X
Rgb_2 = X
Rgb_3 = X
Rgb_4 = X
Waitus 800                                                  '�������
Next X
Waitms 4

For X = 255 To 1 Step -1
Rgb_1 = X
Rgb_2 = X
Rgb_3 = X
Rgb_4 = X
Waitus 800                                                  '�������
Next X

Waitms 12
Return
'%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
M6:
   For X = 1 To 255
Rgb_1 = X
Rgb_2 = 255 - X
Rgb_3 = X
Rgb_4 = 255 - X
Waitus 200                                                  '�������
Next X

For X = 255 To 1 Step -1
Rgb_1 = X
Rgb_2 = 255 - X
Rgb_3 = X
Rgb_4 = 255 - X
Waitus 200                                                  '�������
Next X

Return
'%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
M7:
Rgb_1 = 255
Rgb_2 = 255
Rgb_3 = 255
Rgb_4 = 255

For X = 255 To 1 Step -1
Rgb_1 = X
Waitus 800                                                  '�������
Next X

For X = 1 To 255
Rgb_1 = X
Waitus 800                                                  '�������
Next X

For X = 255 To 1 Step -1
Rgb_2 = X
Waitus 800                                                  '�������
Next X

For X = 1 To 255
Rgb_2 = X
Waitus 800                                                  '�������
Next X

For X = 255 To 1 Step -1
Rgb_3 = X
Waitus 800                                                  '�������
Next X

For X = 1 To 255
Rgb_3 = X
Waitus 800                                                  '�������
Next X

For X = 255 To 1 Step -1
Rgb_4 = X
Waitus 800                                                  '�������
Next X

For X = 1 To 255
Rgb_4 = X
Waitus 800                                                  '�������
Next X

Return
'%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
'###############################################################################
'!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
'0000000000000000000000000000000000000000000000000000000000000000000000000000000
Pwm_out:
Incr Tik
 If Tik = 255 Then
  Tik = 0
  Porta = &B00001111
 End If
   If Tik > Rgb_1 Then Porta.0 = 0
   If Tik > Rgb_2 Then Porta.1 = 0
   If Tik > Rgb_3 Then Porta.3 = 0
   If Tik > Rgb_4 Then Porta.2 = 0
Return
'+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++