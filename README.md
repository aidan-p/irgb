# irgb
Arduino/ESP32 project that is designed to control any RGB lights that are supported by the FastLED library with an IR receiver.

Has not been tested on any official hardware, but the supplied files allow the project to be ran on the Wokwi simulators.

The .ino file may need remappings to be done if your remote does not output the same values, or if your remote does not have the same number of options.

As of now, the project supports the following options:
  1. Red
  2. Green
  3. Blue
  4. Purple
  5. White
  6. Christmas (Red and Green)
  7. Starry Night-White LEDs will randomly appear, then slowly fade out.
  8. Rainbow
  9. Light Blue

Notes: If you want the ends to sync up for rainbow mode and for starry night to properly display, then make sure to adjust the number of LEDs defined in the program.

![image](https://github.com/user-attachments/assets/c980cae9-25d8-4bf6-838d-19c993758cbe)
