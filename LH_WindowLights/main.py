import time
from neopixel import Neopixel
from machine import Pin, Timer, ADC, WDT
import random
import math

GPIO_PIN_DOOR_RGB   = 14
GPIO_PIN_WINDOW_RGB = 8
GPIO_PIN_LDR        = 28
GPIO_PIN_WDT_EN     = 7

door_numpix   = 24
window_numpix = 4

# 32 LED strip connected to X8.
#p = machine.Pin.board.

door_strip = Neopixel(door_numpix, 0, GPIO_PIN_DOOR_RGB, "GRB")
window_rgb = Neopixel(window_numpix, 1, GPIO_PIN_WINDOW_RGB, "GRB")
wdt_enable = Pin(GPIO_PIN_WDT_EN, Pin.IN, Pin.PULL_UP)
ldr_pin    = Pin(GPIO_PIN_LDR, Pin.IN)

ldr_val        = 0
light_on       = True
door_show_done = True

RP2040_MAX_WDT_TIMEOUT = 8388



BLACK  = (0, 0, 0)
RED    = (255, 0, 0)
YELLOW = (255, 150, 0)
GREEN  = (0, 255, 0)
CYAN   = (0, 255, 255)
BLUE   = (0, 0, 255)
PURPLE = (180, 0, 255)
INDIGO = (75, 0, 130)
VIOLET = (138, 43, 226)
ORANGE = (255, 165, 0)
WHITE  = (255,255,255)

WINDOW1 = [255,255,32]
WINDOW2 = [255,255,80]
WINDOW3 = [255,100,64]
WINDOW4 = [200,16,16]
WINDOW0 = [0,0,0]


COLORS = (BLACK, RED, YELLOW, GREEN, CYAN, BLUE, PURPLE, WHITE)
WIN_COLORS = [WINDOW1, WINDOW2, WINDOW3, WINDOW0, WINDOW0] 

window_color = [[0,0,0],[0,0,0],[0,0,0],[0,0,0]]
print (window_color)
window_color_pos = [0,100,200,300]
window_iterations_per_step = [2,3,5,10]
window_step = [0,0,0,0]


# print(color_fi[1])
      
white_100 = (255,255,255)
# white_75 = list(map(operator.mul, white_100, [0.75]*4))
white_75 = [ int(x * 25 / 100) for x in white_100]
white_50 = [ int(x * 10 / 100) for x in white_100]
white_25 = [ int(x * 5 / 100) for x in white_100]

#print (white_75)

black = (0, 0, 0)
position = 0
#colors_rgbw = (red, orange, white_100, black, yellow, green, blue, indigo, violet, white)
#color_pos = (white_100, white_100, white_75, white_50, white_25)
color_pos = [0,0,0]*5 
# same colors as normaln rgb, just 0 added at the end
#colors_rgbw = [color+tuple([0]) for color in colors_rgb]
#colors_rgbw.append((0, 0, 0, 255))

# uncomment colors_rgb if you have RGB strip
# colors = colors_rgb
# colors = colors_rgbw

run_scheme = 0
effect_scheme = 1
run_time   = 10
seconds    = 0
scheme_run_time = [10,10,20,60,20,2]

window_tim = Timer()
door_tim   =  Timer()
schedule_tim  = Timer()
measure_tim   = Timer()


def window_tick(timer):
    global window_color 
    global window_color_pos
    global window_iterations_per_step
    global window_step
    
    #c = 0
    #if windx >= window_numpix-1:
    #    windx = 0
    #else:
    #    windx = windx + 1
        
    for windx in range(window_numpix):
        window_step[windx] = window_step[windx] + 1
        if window_step[windx] > window_iterations_per_step[windx]:
            window_step[windx] = 0
        window_color_pos[windx] = window_color_pos[windx] + 1
        if window_color_pos[windx] > 399:
            window_color_pos[windx] = 0
        color_indx1 = math.floor( window_color_pos[windx] / 100)
        color_indx2 = color_indx1 + 1
        if color_indx2 > window_numpix -1:
            color_indx2 = 0
        dx = (window_color_pos[windx] % 100) / 100.0
        for c in range(3):
            window_color[windx][c] = int(WIN_COLORS[color_indx1][c] + (WIN_COLORS[color_indx2][c] - WIN_COLORS[color_indx1][c]) * dx)         
        # print('Window: ',windx, window_color_pos[windx], color_indx1, color_indx2, window_color[windx])  
        window_rgb.set_pixel(windx, window_color[windx])
    window_rgb.show()
    # time.sleep(0.5)
    #print('---')  

def door_tick(timer):
    global door_show_done
    
    door_strip.show()
    door_show_done = True

def schedule_tick(timer):
    global run_scheme
    global effect_scheme
    global run_time
    global seconds
    global light_on
    seconds = seconds + 1
    if run_scheme == 99:
        seconds = run_time +1
        
    if seconds > run_time:
        if run_scheme > 0:
            run_scheme = 0
        else:
            effect_scheme = effect_scheme +1
            if effect_scheme > 1:
                effect_scheme = 0
            run_scheme = effect_scheme
        run_time = scheme_run_time[run_scheme]
        seconds = 0
        print("run_scheme=",run_scheme, "run_time=", run_time)
        
dark_cntr = 100  
  
def measure_tick(timer):
    global light_on
    global run_scheme
    global dark_cntr
    
    ldr_adc = ADC(ldr_pin)
    ldr_val = ldr_adc.read_u16()
    print("LDR: ",ldr_val)
    if ldr_val < 20000:
        if dark_cntr < 200:
            dark_cntr = dark_cntr + 1
    else:
        if dark_cntr > 0:
            dark_cntr = dark_cntr - 1
    print ("dark_cntr = ", dark_cntr)      
    if dark_cntr > 120:
        light_on = True
        print('light on')
    elif dark_cntr < 80:       
        light_on = False
        print('light off')

        
      
 
        
          
        
schedule_tim.init(freq=1.0, mode=Timer.PERIODIC, callback=schedule_tick)
measure_tim.init(freq=0.2, mode=Timer.PERIODIC, callback=measure_tick)
window_tim.init(freq=10.0, mode=Timer.PERIODIC, callback=window_tick)
door_tim.init(freq=10.0, mode=Timer.PERIODIC, callback=door_tick)

door_strip.brightness(255)
window_rgb.brightness(200)

if wdt_enable():
    print ("Watchdog enabled")
    wdt = WDT(timeout=RP2040_MAX_WDT_TIMEOUT)

def wdt_go_to_sleep():
    if wdt_enable():
        wdt.feed()
    
def color_chase(color, wait):
    for i in range(door_numpix):
        set_pixel(i, color)
        time.sleep(wait)
        door_pixel.show()
    time.sleep(0.2)
 
def wheel(pos):
    # Input a value 0 to 255 to get a color value.
    # The colours are a transition r - g - b - back to r.
    if pos < 0 or pos > 255:
        return (0, 0, 0)
    if pos < 85:
        return (255 - pos * 3, pos * 3, 0)
    if pos < 170:
        pos -= 85
        return (0, 255 - pos * 3, pos * 3)
    pos -= 170
    return (pos * 3, 0, 255 - pos * 3)
 
 
def rainbow_cycle(wait):
    for j in range(255):
        for i in range(door_numpix):
            rc_index = (i * 256 // door_numpix) + j
            door_strip.set_pixel(i, wheel(rc_index & 255))
    door_show_done = False
        #door_strip.show()
        #time.sleep(wait)


def light_color_shades():
    for step in range(10):
        for pix in range(door_numpix):
            # print(random.randint(0,255))
            c = (random.randint(0,255),random.randint(0,255),random.randint(0,128), 0)
            door_strip.set_pixel(pix, c)
        door_strip.show()
        time.sleep(0.5)
 
def light_random_scatter():
    for step in range(10):
        
        for pix in range(door_numpix):
            # print(random.randint(0,255))
            n=random.randint(0,255)
            if (n > 220):
                c = BLUE
            elif (n > 128):
                c = YELLOW
            elif n > 120:
                c = INDIGO
            elif n > 100:
                c = WHITE
            elif n > 80:
                c = RED
            else:
                c = BLACK

            
            # c = (random.randint(0,255),random.randint(0,255),random.randint(0,128), 0)
            door_strip.set_pixel(pix, c)
        door_show_done = False    
        #door_strip.show()
        #show_if_dark()
        #time.sleep(0.05)
    #run_scheme = 99
 
 
def print_formatted(number):
    for i in range(1,number+1):
        temp=("{0:32b}".format(i))
        print(temp)

#print_formatted(128)


def light_all_off(show):
    for pix in range(door_numpix):
        door_strip.set_pixel(pix, (0,0,0,0))
    if show:
        strip.show()
        
def show_if_dark():
    global light_on
    
    if light_on:
        strip.show()
    else:
        light_all_off(True) 
        
        

def check_light():
    global run_scheme
    global light_on
    light_all_off(True)
    time.sleep(1.0)
    ldr_adc = ADC(ldr_pin)
    ldr_val = ldr_adc.read_u16()
    print("LDR: ",ldr_val)
    if ldr_val < 12000:
        light_on = False
        print('light off')
    else:
        light_on = True
        print('light on')
    run_scheme = 99
 
    
while True:
    #window_tick()
    
    while run_scheme == 0:
        if door_show_done:
            rainbow_cycle(0.01)
        wdt_go_to_sleep()    
        #print("normal")
    while run_scheme == 1:
        if door_show_done:
            light_random_scatter()
        wdt_go_to_sleep()
         #print("scatter")


          


