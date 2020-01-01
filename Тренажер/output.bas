Out_out:
   Select Case Ch
  'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      Case 1 : Gosub Out_choose_w
                  Shiftout Porta.6 , Porta.7 , Disp_w , 1
      Case 5 : Gosub Out_choose_w
                  Shiftout Porta.4 , Porta.5 , Disp_w , 1
      Case 9 : Gosub Out_choose_w
                  Shiftout Porta.2 , Porta.3 , Disp_w , 1
     Case 13 : Gosub Out_choose_w
                  Shiftout Porta.0 , Porta.1 , Disp_w , 1
  'BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
      Case 2 : N_on = N_on + 5
                  Gosub Out_choose_w : Shiftout Porta.6 , Porta.7 , Disp_w , 1       ': Toggle Portf.2
      Case 6 : N_on = N_on + 5
                  Gosub Out_choose_w : Shiftout Porta.4 , Porta.5 , Disp_w , 1
     Case 10 : N_on = N_on + 5
                  Gosub Out_choose_w : Shiftout Porta.2 , Porta.3 , Disp_w , 1
     Case 14 : N_on = N_on + 5
                  Gosub Out_choose_w : Shiftout Porta.0 , Porta.1 , Disp_w , 1
  'CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
      Case 3 : Gosub Out_c
                  Shiftout Porta.6 , Porta.7 , Disp_w , 1 : Shiftout Portc.7 , Portc.6 , Disp_b , 1
      Case 7 : Gosub Out_c
                  Shiftout Porta.4 , Porta.5 , Disp_w , 1 : Shiftout Portc.5 , Portc.4 , Disp_b , 1
     Case 11 : Gosub Out_c
                  Shiftout Porta.2 , Porta.3 , Disp_w , 1 : Shiftout Portc.3 , Portc.2 , Disp_b , 1
     Case 15 : Gosub Out_c
                  Shiftout Porta.0 , Porta.1 , Disp_w , 1 : Shiftout Portc.1 , Portc.0 , Disp_b , 1
  'DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD
      Case 4 : Gosub Out_choose_b
                  Shiftout Portc.7 , Portc.6 , Disp_b , 1
      Case 8 : Gosub Out_choose_b
                  Shiftout Portc.5 , Portc.4 , Disp_b , 1
     Case 12 : Gosub Out_choose_b
                  Shiftout Portc.3 , Portc.2 , Disp_b , 1
     Case 16 : Gosub Out_choose_b
                  Shiftout Portc.1 , Portc.0 , Disp_b , 1
   End Select

 Pulseout Portg , 2 , 5                                     'Pulse_pin     PORTG.2

Return
'//////////////////////////////////////////////////////////////////
Out_c:
   Select Case N_on
      Case 1 : Disp_w = 0
                  N_on = N_on + 5 : Gosub Out_choose_b
      Case 2 : Disp_w = 0
                  N_on = N_on + 5 : Gosub Out_choose_b
      Case 3 : Disp_w = 0
                  N_on = N_on + 5 : Gosub Out_choose_b
      Case 4 : N_on = 11
                  Gosub Out_choose_w : N_on = 9 : Gosub Out_choose_b
      Case 5 : N_on = 12
                  Gosub Out_choose_w : N_on = 10 : Gosub Out_choose_b
   End Select
Return
'//////////////////////////////////////////////////////////////////
Out_choose_b:
 If Flag_dot = 0 Then
   Disp_b = Lookup(n_on , Mass_point_b)
 Else
   Disp_b = Lookup(n_on , Mass_bar_b)
 End If
Return
'//////////////////////////////////////////////////////////////////
Out_choose_w:
 If Flag_dot = 0 Then
   Gosub Mass_point_w
 Else
   Gosub Mass_bar_w
 End If
Return
'=================================================================


Mass_point_w:

If N_on = 1 Then
   Disp_w = &B1000011100000001                              'A - 1
Elseif N_on = 2 Then
   Disp_w = &B1000101100000001                              'A - 2
Elseif N_on = 3 Then
   Disp_w = &B1001001100000001                              'A - 3
Elseif N_on = 4 Then
   Disp_w = &B1010001100000001                              'A - 4
Elseif N_on = 5 Then
   Disp_w = &B1100001100000001                              'A - 5

Elseif N_on = 6 Then
   Disp_w = &B1000001100000011                              'B - 1
Elseif N_on = 7 Then
   Disp_w = &B1000001100000101                              'B - 2
Elseif N_on = 8 Then
   Disp_w = &B1000001100001001                              'B - 3
Elseif N_on = 9 Then
   Disp_w = &B1000001100010001                              'B - 4
Elseif N_on = 10 Then
   Disp_w = &B1000001100100001                              'B - 5

Elseif N_on = 11 Then
   Disp_w = &B1000001101000001                              'C - 4
Elseif N_on = 12 Then
   Disp_w = &B1000001110000001                              'C - 5
End If

Return

Mass_point_b:
Data 0
Data &B00000001                                             'D - 1
Data &B00000010                                             'D - 2
Data &B00000100                                             'D - 3
Data &B00001000                                             'D - 4
Data &B00010000                                             'D - 5

Data &B00100000                                             'C - 1
Data &B01000000                                             'C - 2
Data &B10000000                                             'C - 3
Data 0                                                      'C - 4
Data 0                                                      'C - 5

'*******************************************************************************
Mass_bar_w:
If N_on = 1 Then
   Disp_w = &B1000011100000001                              'A - 1
Elseif N_on = 2 Then
   Disp_w = &B1000111100000001                              'A - 2
Elseif N_on = 3 Then
   Disp_w = &B1001111100000001                              'A - 3
Elseif N_on = 4 Then
   Disp_w = &B1011111100000001                              'A - 4
Elseif N_on = 5 Then
   Disp_w = &B1111111100000001                              'A - 5

Elseif N_on = 6 Then
   Disp_w = &B1000001100000011                              'B - 1
Elseif N_on = 7 Then
   Disp_w = &B1000001100000111                              'B - 2
Elseif N_on = 8 Then
   Disp_w = &B1000001100001111                              'B - 3
Elseif N_on = 9 Then
   Disp_w = &B1000001100011111                              'B - 4
Elseif N_on = 10 Then
   Disp_w = &B1000001100111111                              'B - 5

Elseif N_on = 11 Then
   Disp_w = &B1000001101000001                              'C - 4
Elseif N_on = 12 Then
   Disp_w = &B1000001111000001                              'C - 5
End If
Return


Mass_bar_b:
Data 0
Data &B00000001                                             'D - 1
Data &B00000011                                             'D - 2
Data &B00000111                                             'D - 3
Data &B00001111                                             'D - 4
Data &B00011111                                             'D - 5

Data &B00100000                                             'C - 1
Data &B01100000                                             'C - 2
Data &B11100000                                             'C - 3
Data &B11100000                                             'C - 4
Data &B11100000                                             'C - 5


'-------------------------------------------------------------------------------
Out_test:
   X = &B00100001
   Y = &B1000010000000010
   Gosub Out_test_set
   Pulseout Portg , 2 , 5
 Waitms 500
   X = &B01000010
   Y = &B1000100000000100
   Gosub Out_test_set
   Pulseout Portg , 2 , 5
 Waitms 500
   X = &B10000100
   Y = &B1001000000001000
   Gosub Out_test_set
   Pulseout Portg , 2 , 5
 Waitms 500
   X = &B00001000
   Y = &B1010000001010000
   Gosub Out_test_set
   Pulseout Portg , 2 , 5
 Waitms 500
   X = &B00010000
   Y = &B1100000010100000
   Gosub Out_test_set
   Pulseout Portg , 2 , 5
 Waitms 500

 If Pind.3 = 0 Then : If Flag_dot_ee = 0 Then : Flag_dot_ee = 1 : Else : Flag_dot_ee = 0 : End If : End If

For W = 1 To 25

   X = &B00100001
   Y = &B1000010000000010
   Gosub Out_test_set
   Pulseout Portg , 2 , 5
 Waitms 500
   X = &B01000010
   Y = &B1000100000000100
   Gosub Out_test_set
   Pulseout Portg , 2 , 5
 Waitms 500
   X = &B10000100
   Y = &B1001000000001000
   Gosub Out_test_set
   Pulseout Portg , 2 , 5
 Waitms 500
   X = &B00001000
   Y = &B1010000001010000
   Gosub Out_test_set
   Pulseout Portg , 2 , 5
 Waitms 500
   X = &B00010000
   Y = &B1100000010100000
   Gosub Out_test_set
   Pulseout Portg , 2 , 5
 Waitms 500

Next W
Return

Out_test_set:
   Shiftout Portc.7 , Portc.6 , X , 1
   Shiftout Portc.5 , Portc.4 , X , 1
   Shiftout Portc.3 , Portc.2 , X , 1
   Shiftout Portc.1 , Portc.0 , X , 1
      Shiftout Porta.6 , Porta.7 , Y , 1
      Shiftout Porta.4 , Porta.5 , Y , 1
      Shiftout Porta.2 , Porta.3 , Y , 1
      Shiftout Porta.0 , Porta.1 , Y , 1
Return