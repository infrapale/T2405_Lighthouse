'Lighthouse Indoor Lights
'
Print "Lighthouse Indoor Lights TH 2022"
Option explicit
Option autorun on
WatchDog 60000
If MM.Watchdog Then
  Print "Watchdog Reset"
EndIf

Dim b%(4) = (RGB(127,100,10),RGB(GREEN),RGB(BLUE),RGB(YELLOW),RGB(CYAN))
Dim win%(4)
win%(0)=RGB(255,255,32)
win%(1)=RGB(255,255,80)
win%(2)=RGB(255,100,64)
win%(3)=RGB(200,16,16)



Const NBR_PIX= 12
Const NBR_WINDOWS = 4
Dim integer r_g_b(NBR_PIX)
Dim integer win_rgb(NBR_WINDOWS)
Dim cdot(3) As integer


Option explicit
'Const LED_NBR    = 8
'Const LED_BLUE1  = 11


Const LDR_PIN    = 31

Const DARK_IF_LESS  = 2.0
Const MEAS_EVERY_N  = 10

SetPin LDR_PIN, AIN

Dim i As integer

Dim meas_iterations As integer
Dim show_iterations As integer
Dim is_dark As integer
Dim led_indx As integer
Dim led_d As integer
Dim keep_dark As integer
led_indx = 0: led_d = 1
is_dark = 1: keep_dark = 0

Sub wheel Position
  Select Case Position
    Case IS < 0, IS > 255
      cdot(0) =0: c(1)=0: c(2)=0
    Case is < 85
      cdot(0) = Position * 3
      cdot(1) = 255 -Position * 3
      cdot(2) = 0
    Case is < 170
      Position = Position - 85
      cdot(0) = 255 -Position * 3
      cdot(1) = 0
      cdot(2) = Position * 3
    Case Else
      Position = Position -170
      cdot(0) = 0
      cdot(1) = Position * 3
      cdot(2) = 255 -Position * 3

  End Select
End Sub


Sub rgb_show
  Bitbang WS2812 O,GP14,NBR_PIX,r_g_b()
End Sub

Sub show_window
  Bitbang WS2812 O,GP8,NBR_WINDOWS,win_rgb()
End Sub

Sub light_show
  Local Dim integer i,j,k,n,indx
  For n = 0 To 1
    For j = 0 To 255
      For i = 0 To NBR_PIX
        indx = (i * 256 / NBR_PIX)+j
        wheel(indx And 255)
        If Not is_dark Or keep_dark Then
          r_g_b(i) =RGB(0,0,0)
        Else
          r_g_b(i) =(RGB(cdot(0),cdot(1),cdot(2)))
        EndIf
        rgb_show()
        'Print n,j,i,k,cdot(k)
      Next i
      For i = 0 To NBR_WINDOWS
        win_rgb(i) = win%(i)
      Next i
      show_window()
    Next j
  Next n
End Sub


show_iterations = 0
While 1 do
  show_iterations = show_iterations + 1
  Print (show_iterations)
  If show_iterations < MEAS_EVERY_N Then
    keep_dark = 0
    Print "do not keep dark"
  Else
    keep_dark = 1
    show_iterations = 0
    Print "keep dark"
  EndIf
  light_show()

  If keep_dark Then
    Print Pin(LDR_PIN)
    If Pin(LDR_PIN) < DARK_IF_LESS Then
      is_dark = 1
    Else
      is_dark = 1
    EndIf
  EndIf
  WatchDog 60000
Loop                                                                                                                               