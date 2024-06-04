import time
from neopixel import Neopixel
from machine import Pin, Timer, ADC
import random

rows   = 3
pix_per_round = 26
numpix = pix_per_round * rows
strip = Neopixel(numpix, 0, 14, "RGBW")

ldr_pin = Pin(29,mode = Pin.IN)
ldr_val = 0
light_on = True

red = (255, 0, 0, 0)
orange = (255, 165, 0, 0)
yellow = (255, 150, 0, 0)
green = (0, 255, 0, 0 )
blue = (0, 0, 255, 0)
indigo = (75, 0, 130, 0)
violet = (138, 43, 226, 0)
white = (0, 0, 0, 255)

color_fi = (
    (55,  0,  0,  0),
    (155,  0,  0,  0),
    (255,  0,  0,  0),
    (255,  0,  0,  0),
    (255,  0,  0,  0),
    (255,  0,  0,  0),
    (255,  0,  0,  0),
    (255,  0,  0,  0),
    (255,255,  0,  0),
    (  0,255,  0,  0),
    (  0,255,  0,  0),
    (  0,255,  0,  0),
    (  0,255,  0,  0),
    (  0,255,  0,  0),
    (  0,255,  0,  0),
    (  0,255,  0,  0),
    ( 55, 55, 55,255),
    (255,255,255,255),
    (255,255,255,255),
    (255,255,255,255),
    (255,255,255,255),
    (255,255,255,255),
    (255,255,255,255),
    (255,255,255,255),
    (255,255,255,255),
    (255, 55, 55, 55)
)
# print(color_fi[1])
      
white_100 = (255,255,255,255)
# white_75 = list(map(operator.mul, white_100, [0.75]*4))
white_75 = [ int(x * 25 / 100) for x in white_100]
white_50 = [ int(x * 10 / 100) for x in white_100]
white_25 = [ int(x * 5 / 100) for x in white_100]

#print (white_75)

black = (0, 0, 0, 0)
position = 0
#colors_rgbw = (red, orange, white_100, black, yellow, green, blue, indigo, violet, white)
#color_pos = (white_100, white_100, white_75, white_50, white_25)
color_pos = [0,0,0,0]*5 
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
scheme_run_time = [300,10,20,60,20,2]

packman_sleep = 0.1

rotate_tim = Timer()
schedule_tim  = Timer()


def rotate_tick(timer):
    global position
    position = position + 1
    if position >= pix_per_round:
        position = 0
    # print(position)

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
            if effect_scheme > 5:
                effect_scheme = 1
            run_scheme = effect_scheme
        run_time = scheme_run_time[run_scheme]
        seconds = 0
        print("run_scheme=",run_scheme, "run_time=", run_time)
        
        
 
        
          
        
rotate_tim.init(freq=10.0, mode=Timer.PERIODIC, callback=rotate_tick)
schedule_tim.init(freq=1.0, mode=Timer.PERIODIC, callback=schedule_tick)


strip.brightness(255)
iterations = 20


def light_color_shades():
    for step in range(10):
        for pix in range(numpix):
            # print(random.randint(0,255))
            c = (random.randint(0,255),random.randint(0,255),random.randint(0,128), 0)
            strip.set_pixel(pix, c)
        strip.show()
        time.sleep(0.5)
    
def print_formatted(number):
    for i in range(1,number+1):
        temp=("{0:32b}".format(i))
        print(temp)

#print_formatted(128)


def light_all_off(show):
    for pix in range(numpix):
        strip.set_pixel(pix, (0,0,0,0))
    if show:
        strip.show()
        
def show_if_dark():
    global light_on
    
    if light_on:
        strip.show()
    else:
        light_all_off(True) 
        
        
def light_wave():
    global run_scheme
    for step in range(10):
        row = 0
        sign = 1
        light_all_off(False)
        for sector in range(pix_per_round):
            light_all_off(False)
            strip.set_pixel(row*pix_per_round + sector, (0,0,0,255))
            show_if_dark()
            time.sleep(0.02)               
            row = row + sign
            if (row >= rows):
                sign = -1
                row = row + sign
            elif row < 0:
                sign = 1
                row = 1
    run_scheme = 99


def light_packman():
    global run_scheme
    green_ball = [12,15,3]
    packman    = [28,7,7]
    green_ball_0 = 7
    mask = 1
    mask_over = 1 << (pix_per_round)
    mask_all = 0
    sleep = 0.4
    
    mask = 1
    for sector in range(pix_per_round):
        mask_all = mask_all | mask
        mask = mask << 1

    for row in range(rows):
        green_ball[row] = green_ball[row] << 6
        
    for step in range(pix_per_round * 20):
        light_all_off(False)
        # print(green_ball)
        
        mask = 1
        for sector in range(pix_per_round):
            #print(green_ball)
            
            for row in range(rows):
                if (green_ball[row] & mask) != 0:
                    strip.set_pixel(row*pix_per_round + sector, (255,0,0,0))
                if (packman[row] & mask) != 0:
                    strip.set_pixel(row*pix_per_round + sector, (0,255,0,0))
            mask = mask << 1
                
        show_if_dark()
        
            
        for row in range(rows):
            green_ball[row] = green_ball[row] << 1    
            if (green_ball[row] & mask_over) != 0:
                green_ball[row] = (green_ball[row] & mask_all) | 1
                
            packman[row] = packman[row] << 1    
            if (packman[row] & mask_over) != 0:
                packman[row] = (packman[row] & mask_all) | 1

        time.sleep(sleep)
        sleep = sleep*0.95
        
    run_scheme = 99

def light_random_scatter():
    for step in range(10):
        
        for pix in range(numpix):
            # print(random.randint(0,255))
            n=random.randint(0,255)
            if (n > 128):
                c = (0,0,0,255)
            elif n > 120:
                c = (0,255,0,0)
            elif n > 100:
                c = (0,255,0,0)
            elif n > 80:
                c = (0,0,255,0)
            else:
                c = (0,0,0,0)
        
            
            # c = (random.randint(0,255),random.randint(0,255),random.randint(0,128), 0)
            strip.set_pixel(pix, c)
        show_if_dark()
        time.sleep(0.05)
    #run_scheme = 99


def light_house_normal():
    for sector in range(pix_per_round):
        # print(sector)
        color_x = color_fi[sector]
        color_pos[0] = color_x
        color_pos[1] = [ int(x * 66 / 100) for x in color_x]
        color_pos[2] = [ int(x * 33 / 100) for x in color_x]
        color_pos[3] = [ int(x * 10 / 100) for x in color_x]
        color_pos[4] = [ int(x * 5 / 100) for x in color_x]
        # print(color_pos)
        dist1 = abs(sector -position)
        dist2 = abs(sector-position+pix_per_round)
        dist  = min(dist1,dist2)
        if dist < 5:
            color = color_pos[dist]
            #print(color)
        else:
            color = black
        for row in range(rows):    
            strip.set_pixel(row*pix_per_round + sector, color)
    show_if_dark()
 
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
    while run_scheme == 0:
        light_house_normal()
        # light_wave()
        # light_packman()
    while run_scheme == 1:
        light_color_shades()
    while run_scheme == 2:
        light_random_scatter()
    while run_scheme == 3:
        light_packman()
    while run_scheme == 4:
        light_wave()
    while run_scheme == 5:
        check_light()
        


          

