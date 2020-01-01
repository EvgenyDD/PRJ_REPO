'1111111111111111111111111111111111111111111111111111111111111111111111111111111
Button_1:
 Reset Col1 : Reset Col2 : Reset Col3 : Reset Col4 : Reset Cold1 : Reset Cold2 : Reset Cold3 : Reset Cold4
 Set Rowd1
 Gosub Checkmass
   Waitms 2
      Reset Rowd1
      Set Col1 : Set Cold1
         If Row1 = 1 Then
                  If Led_on(1) = 0 Then : Led_on(1) = 1 : Else : If Z > 1 Then : Led_on(1) = 0 : End If : End If : N_mass = 1
         End If
      Reset Col1 : Reset Cold1
      Set Rowd1
   Waitms 2
      Reset Rowd1
      Set Col2 : Set Cold2
         If Row1 = 1 Then
                  If Led_on(2) = 0 Then : Led_on(2) = 1 : Else : If Z > 1 Then : Led_on(2) = 0 : End If : End If : N_mass = 2
         End If
      Reset Col2 : Reset Cold2
      Set Rowd1
   Waitms 2
      Reset Rowd1
      Set Col3 : Set Cold3
      If Row1 = 1 Then
                  If Led_on(3) = 0 Then : Led_on(3) = 1 : Else : If Z > 1 Then : Led_on(3) = 0 : End If : End If : N_mass = 3
      End If
      Reset Col3 : Reset Cold3
      Set Rowd1
   Waitms 2
      Reset Rowd1
      Set Col4 : Set Cold4
         If Row1 = 1 Then
                  If Led_on(4) = 0 Then : Led_on(4) = 1 : Else : If Z > 1 Then : Led_on(4) = 0 : End If : End If : N_mass = 4
         End If
   Waitms 2
 Set Col1 : Set Col2 : Set Col3 : Set Cold1 : Set Cold2 : Set Cold3

' Set Flag_int_btn
 Eifr = &B11111111
 Flag_int_buttn = 1
Return



'2222222222222222222222222222222222222222222222222222222222222222222222222222222
Button_2:
 Reset Col1 : Reset Col2 : Reset Col3 : Reset Col4 : Reset Cold1 : Reset Cold2 : Reset Cold3 : Reset Cold4
 Set Rowd2
 Gosub Checkmass
   Waitms 2
      Reset Rowd2
      Set Col1 : Set Cold1
         If Row2 = 1 Then
                  If Led_on(5) = 0 Then : Led_on(5) = 1 : Else : If Z > 1 Then : Led_on(5) = 0 : End If : End If : N_mass = 5
         End If
      Reset Col1 : Reset Cold1
      Set Rowd2
   Waitms 2
      Reset Rowd2
      Set Col2 : Set Cold2
         If Row2 = 1 Then
                  If Led_on(6) = 0 Then : Led_on(6) = 1 : Else : If Z > 1 Then : Led_on(6) = 0 : End If : End If : N_mass = 6
         End If
      Reset Col2 : Reset Cold2
      Set Rowd2
   Waitms 2
      Reset Rowd2
      Set Col3 : Set Cold3
      If Row2 = 1 Then
                  If Led_on(7) = 0 Then : Led_on(7) = 1 : Else : If Z > 1 Then : Led_on(7) = 0 : End If : End If : N_mass = 7
      End If
      Reset Col3 : Reset Cold3
      Set Rowd2
   Waitms 2
      Reset Rowd2
      Set Col4 : Set Cold4
         If Row2 = 1 Then
                  If Led_on(8) = 0 Then : Led_on(8) = 1 : Else : If Z > 1 Then : Led_on(8) = 0 : End If : End If : N_mass = 8
         End If
   Waitms 2
 Set Col1 : Set Col2 : Set Col3 : Set Cold1 : Set Cold2 : Set Cold3

' Set Flag_int_btn
 Eifr = &B11111111
 Flag_int_buttn = 1
Return


'3333333333333333333333333333333333333333333333333333333333333333333333333333333
Button_3:
 Reset Col1 : Reset Col2 : Reset Col3 : Reset Col4 : Reset Cold1 : Reset Cold2 : Reset Cold3 : Reset Cold4
 Set Rowd3
 Gosub Checkmass
   Waitms 2
      Reset Rowd3
      Set Col1 : Set Cold1
         If Row3 = 1 Then
                  If Led_on(9) = 0 Then : Led_on(9) = 1 : Else : If Z > 1 Then : Led_on(9) = 0 : End If : End If : N_mass = 9
         End If
      Reset Col1 : Reset Cold1
      Set Rowd3
   Waitms 2
      Reset Rowd3
      Set Col2 : Set Cold2
         If Row3 = 1 Then
                  If Led_on(10) = 0 Then : Led_on(10) = 1 : Else : If Z > 1 Then : Led_on(10) = 0 : End If : End If : N_mass = 10
         End If
      Reset Col2 : Reset Cold2
      Set Rowd3
   Waitms 2
      Reset Rowd3
      Set Col3 : Set Cold3
      If Row3 = 1 Then
                  If Led_on(11) = 0 Then : Led_on(11) = 1 : Else : If Z > 1 Then : Led_on(11) = 0 : End If : End If : N_mass = 11
      End If
      Reset Col3 : Reset Cold3
      Set Rowd3
   Waitms 2
      Reset Rowd3
      Set Col4 : Set Cold4
         If Row3 = 1 Then
                  If Led_on(12) = 0 Then : Led_on(12) = 1 : Else : If Z > 1 Then : Led_on(12) = 0 : End If : End If : N_mass = 12
         End If
   Waitms 2
 Set Col1 : Set Col2 : Set Col3 : Set Cold1 : Set Cold2 : Set Cold3

' Set Flag_int_btn
 Eifr = &B11111111
 Flag_int_buttn = 1
Return


'4444444444444444444444444444444444444444444444444444444444444444444444444444444
Button_4:
 Reset Col1 : Reset Col2 : Reset Col3 : Reset Col4 : Reset Cold1 : Reset Cold2 : Reset Cold3 : Reset Cold4
 Set Rowd4
 Gosub Checkmass
   Waitms 2
      Reset Rowd4
      Set Col1 : Set Cold1
         If Row4 = 1 Then
                  If Led_on(13) = 0 Then : Led_on(13) = 1 : Else : If Z > 1 Then : Led_on(13) = 0 : End If : End If : N_mass = 13
         End If
      Reset Col1 : Reset Cold1
      Set Rowd4
   Waitms 2
      Reset Rowd4
      Set Col2 : Set Cold2
         If Row4 = 1 Then
                  If Led_on(14) = 0 Then : Led_on(14) = 1 : Else : If Z > 1 Then : Led_on(14) = 0 : End If : End If : N_mass = 14
         End If
      Reset Col2 : Reset Cold2
      Set Rowd4
   Waitms 2
      Reset Rowd4
      Set Col3 : Set Cold3
      If Row4 = 1 Then
                  If Led_on(15) = 0 Then : Led_on(15) = 1 : Else : If Z > 1 Then : Led_on(15) = 0 : End If : End If : N_mass = 15
      End If
      Reset Col3 : Reset Cold3
      Set Rowd4
   Waitms 2
      Reset Rowd4
      Set Col4 : Set Cold4
         If Row4 = 1 Then
                  If Led_on(16) = 0 Then : Led_on(16) = 1 : Else : If Z > 1 Then : Led_on(16) = 0 : End If : End If : N_mass = 16
         End If
   Waitms 2
 Set Col1 : Set Col2 : Set Col3 : Set Cold1 : Set Cold2 : Set Cold3

' Set Flag_int_btn
 Eifr = &B11111111
 Flag_int_buttn = 1
Return






'SETTINGS___SETTINGS___SETTINGS___SETTINGS___SETTINGS___SETTINGS___SETTINGS_____
Button_5:
                  'PLAY___PLAY___PLAY___PLAY___PLAY___PLAY___PLAY___PLAY___PLAY_
If M = 1 Then
   '///////////////////////////
      Reset Col1 : Reset Col2 : Reset Col3 : Reset Col4 : Reset Cold1 : Reset Cold2 : Reset Cold3 : Reset Cold4
      Set Rowd5

   Waitms 2
      Reset Rowd5
      Set Col1 : Set Cold1
         If Row5 = 1 Then
                   If N_prog = 1 Then : N_prog = 25 : Else : Decr N_prog : End If : N_step = 0 : Count1 = 1 : Countm = 1
                   E_mode(n_prog) = Ee_mode(n_prog)
                   E_mass(n_prog) = Ee_mass(n_prog)
                   E_time(n_prog) = Ee_time(n_prog)
                   Stop Timer0
         End If
      Reset Col1 : Reset Cold1
      Set Rowd5
   Waitms 50
      Reset Rowd5
      Set Col4 : Set Cold4
         If Row5 = 1 Then
                   If N_prog = 25 Then : N_prog = 1 : Else : Incr N_prog : End If : N_step = 0 : Count1 = 1 : Countm = 1
                   E_mode(n_prog) = Ee_mode(n_prog)
                   E_mass(n_prog) = Ee_mass(n_prog)
                   E_time(n_prog) = Ee_time(n_prog)
                   Stop Timer0
         End If
   Waitms 50
   Set Col1 : Set Col2 : Set Col3 : Set Cold1 : Set Cold2 : Set Cold3
   '///////////////////////////
                  'PROG___PROG___PROG___PROG___PROG___PROG___PROG___PROG___PROG_
Elseif M = 2 Then
   '///////////////////////////
   Reset Col1 : Reset Col2 : Reset Col3 : Reset Col4 : Reset Cold1 : Reset Cold2 : Reset Cold3 : Reset Cold4
   Set Rowd5

   Waitms 2
      Reset Rowd5
      Set Col1 : Set Cold1
         If Row5 = 1 Then
                  If N_step = 1 Then : N_step = 50 : Else : Decr N_step : End If
                  Flag_set = 0
                  Reset Portf.0 : Reset Portf.2
                  Waitms 100
         End If
      Reset Col1 : Reset Cold1
      Set Rowd5
   Waitms 2
      Reset Rowd5
      Set Col2 : Set Cold2
         If Row5 = 1 Then
                  Waitms 150 : Reset Col2 : Reset Cold2 : Set Col3 : Set Cold3
                  If Row5 = 1 Then
                     Flag_set = 3
                     If N_step <> 1 Then : Portf.0 = 1 : Portf.2 = 1 : End If
                     Waitms 100
                     Reset Col3 : Reset Cold3
                  Else
                     Portf.2 = 1 : Portf.0 = 0
                     Flag_set = 1
                  End If
         End If
      Reset Col2 : Reset Cold2
      Set Rowd5
   Waitms 2
      Reset Rowd5
      Set Col3 : Set Cold3
      If Row5 = 1 Then
                  Waitms 150 : Reset Col3 : Reset Cold3 : Set Col2 : Set Cold2
                  If Row5 = 1 Then
                     Flag_set = 3
                     If N_step <> 1 Then : Portf.0 = 1 : Portf.2 = 1 : End If
                     Waitms 100
                     Reset Col2 : Reset Cold2
                  Else
                     Portf.0 = 1 : Portf.2 = 0
                     Flag_set = 2
                        Str_temp = Mid(e_mass(n_prog) , N_step , 1)       'чтенье канала удара
                        Gosub Choose
                        N_mass = R
                  End If
      End If
      Reset Col3 : Reset Cold3
      Set Rowd5
   Waitms 2
      Reset Rowd5
      Set Col4 : Set Cold4
         If Row5 = 1 Then
                  If N_step = 50 Then : N_step = 1 : Else : Incr N_step : End If
                  Flag_set = 0
                  Reset Portf.0 : Reset Portf.2
                  Waitms 100
         End If
   Waitms 2
 Set Col1 : Set Col2 : Set Col3 : Set Cold1 : Set Cold2 : Set Cold3
   '///////////////////////////
                  'PLAY.ALL___PLAY.ALL___PLAY.ALL___PLAY.ALL___PLAY.ALL___PLAY.ALL
Elseif M = 3 Then
    '///////////////////////////
      Reset Col1 : Reset Col2 : Reset Col3 : Reset Col4 : Reset Cold1 : Reset Cold2 : Reset Cold3 : Reset Cold4
      Set Rowd5

   Waitms 2
      Reset Rowd5
      Set Col1 : Set Cold1
         If Row5 = 1 Then
                   If N_repeat = 1 Then : N_repeat = 15 : Else : Decr N_repeat : End If
         End If
      Reset Col1 : Reset Cold1
      Set Rowd5
   Waitms 2
      Reset Rowd5
      Set Col4 : Set Cold4
         If Row5 = 1 Then
                   If N_repeat = 15 Then : N_repeat = 1 : Else : Incr N_repeat : End If
         End If
   Waitms 2
   Set Col1 : Set Col2 : Set Col3 : Set Cold1 : Set Cold2 : Set Cold3
   '///////////////////////////

End If
 Eifr = &B11111111
 Flag_int_buttn = 1
Return




'OPTION___OPTION___OPTION___OPTION___OPTION___OPTION___OPTION___OPTION___OPTION
Button_21:

                  'RANDOM___RANDOM___RANDOM___RANDOM___RANDOM___RANDOM___RANDOM_
If M = 0 Then                                               'в первом режиме включаем/выключаем кнопкой сразу все каналы
   Gosub Checkmass
   If Z = 16 Then
      For W = 2 To 16 : Led_on(w) = 0 : Next W
      Led_on(1) = 1
   Else
      For W = 1 To 16 : Led_on(w) = 1 : Next W
   End If


                  'PLAY___PLAY___PLAY___PLAY___PLAY___PLAY___PLAY___PLAY___PLAY_
Elseif M = 1 Then
   Flag_m_all = 1

                  'PROG___PROG___PROG___PROG___PROG___PROG___PROG___PROG___PROG_
Elseif M = 2 Then                                           'в режиме проги очищаем всю программу
      E_mode(n_prog) = "22222222222222222222222222222222222222222222222222"
      E_mass(n_prog) = "11111111111111111111111111111111111111111111111111"
      E_time(n_prog) = "00000000000000000000000000000000000000000000000000"


                  'PLAY_ALL___PLAY_ALL___PLAY_ALL___PLAY_ALL___PLAY_ALL___PLAY_A
Elseif M = 3 Then
   Flag_m_all = 0

End If

Flag_int_buttn = 2
 Flag_int_buttn_count = 5 * Butn_opros
 'Disable Int3
 'Eifr = &B11111111

Return

'///////////////////////////////////////////////////////////////////////////////
Checkmass:
Z = 0
For W = 1 To 16
   If Led_on(w) = 1 Then
      Q = W
      Z = Z + 1
   End If
Next W
Return