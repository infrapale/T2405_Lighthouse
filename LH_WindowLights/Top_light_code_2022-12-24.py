# SPDX-FileCopyrightText: 2021 ladyada for Adafruit Industries
# SPDX-License-Identifier: MIT

import time
import board 
import neopixel
import digitalio
import math
import ulab.numpy as np


# On CircuitPlayground Express, and boards with built in status NeoPixel -> board.NEOPIXEL
# Otherwise choose an open pin connected to the Data In of the NeoPixel strip, i.e. board.D1
# pixel_pin = board.NEOPIXEL
# https://learn.adafruit.com/getting-started-with-raspberry-pi-pico-circuitpython
# https://docs.circuitpython.org/en/latest/docs/index.html
# https://learn.adafruit.com/ulab-crunch-numbers-fast-with-circuitpython?view=all


pixel_pin = board.GP8

# On a Raspberry pi, use this instead, not all pins are supported
# pixel_pin = board.D18

# The number of NeoPixels
num_pixels = 13

# The order of the pixel colors - RGB or GRB. Some NeoPixels have red and green reversed!
# For RGBW NeoPixels, simply change the ORDER to RGBW or GRBW.
ORDER = neopixel.GRB

pixels = neopixel.NeoPixel(
    pixel_pin, num_pixels, brightness=0.2, auto_write=False, pixel_order=ORDER
)


def wheel(pos):
    # Input a value 0 to 255 to get a color value.
    # The colours are a transition r - g - b - back to r.
    if pos < 0 or pos > 255:
        r = g = b = 0
    elif pos < 85:
        r = int(pos * 3)
        g = int(255 - pos * 3)
        b = 0
    elif pos < 170:
        pos -= 85
        r = int(255 - pos * 3)
        g = 0
        b = int(pos * 3)
    else:
        pos -= 170
        r = 0
        g = int(pos * 3)
        b = int(255 - pos * 3)
    return (r, g, b) if ORDER in (neopixel.RGB, neopixel.GRB) else (r, g, b, 0)


def rainbow_cycle(wait):
    for n in range(255):
        for j in range(255):
            for i in range(num_pixels):
                pixel_index = (i * 256 // num_pixels) + j
                pixels[i] = wheel(pixel_index & 255)
            pixels.show()
            time.sleep(wait)


def normalized_rms_ulab(values):
    # this function works with ndarrays only
    minbuf = np.mean(values)
    values = values - minbuf
    samples_sum = np.sum(values * values)
    return math.sqrt(samples_sum / len(values))

vect2 = np.linspace(-3, 3, num=10)
vect1 = np.linspace(1, 1, num=10)
print(vect1)
print(vect2)
vect3 = vect1/(vect2*vect2)

print(vect3)
vect4 = vect3/np.max(vect3)*255
print(vect4)




while True:
    # Comment this line out if you have RGBW/GRBW NeoPixels
    pixels.fill((25, 0, 0))
    # Uncomment this line if you have RGBW/GRBW NeoPixels
    # pixels.fill((255, 0, 0, 0))
    pixels.show()
    time.sleep(2)

    # Comment this line out if you have RGBW/GRBW NeoPixels
    pixels.fill((0, 25, 0))
    # Uncomment this line if you have RGBW/GRBW NeoPixels
    # pixels.fill((0, 255, 0, 0))
    pixels.show()
    time.sleep(2)

    # Comment this line out if you have RGBW/GRBW NeoPixels
    pixels.fill((0, 0, 25))
    # Uncomment this line if you have RGBW/GRBW NeoPixels
    # pixels.fill((0, 0, 255, 0))
    pixels.show()
    time.sleep(2)

    rainbow_cycle(0.01)  # rainbow cycle with 1ms delay per step

