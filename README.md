# paper sunshine clock
![IMG_0079](https://github.com/user-attachments/assets/cae93059-a808-4a04-b790-e4afc18b27fd)

This project is a clock that tells time in a unique way. The face of the display looks like the sun with 12 rays representing 12 o’clock to 11 o’clock, going clockwise, which turn from black to yellow as the day progresses. Each ray is divided up into half hour segments, so the clock is able to tell you the current half hour. For example in the photo above, the clock is reading between 9 o’clock and 9:30. The display is made out of paper and uses thermochromic paint that changes color with heat. There is a thin circuit board behind the paper which provides the warmth to activate the thermochromic paint, changing the color of the rays. The clock also has Sparkle Mode, which will turn on a random ray every 10 seconds. It provides an entertaining show as the rays turn on and fade away (see [Videos](#Videos) below). The enclosure is 3D printed.

https://github.com/user-attachments/assets/e0f1c668-d030-4747-b99e-f9c4dc16aae8

---
### Reading the clock
The clock has 12 rays, representing 12 hours from 12 o'clock to 11 o'clock. Each ray is divided up into half hour segments. As each half hour of the day passes, the corresponding segment will turn from black to yellow. The clock face is fully yellow between 11:30-11:59. At 12:00, it'll reset. 

<img width="700" alt="face_diagram" src="https://github.com/user-attachments/assets/51da8923-1cb1-4d92-bac2-d24a92117f71" />

---
### How it works
The clock works by using thermochromic paint screen printed onto a piece of Japanese awagami paper. Behind that, is a flex PCB with large resistors mounted on the back. These resistors can be selectively turned on, warming up the paper and activating the thermochromic paint. 
Each ray on the PCB is a copper pour used to spread and hold heat from the resistors. It was designed using Python and an SVG drawing library, and converted into a KiCad footprint.

![display_front_sm](https://github.com/user-attachments/assets/e842a568-bab1-4ef8-acd7-1334f82143dd)
![display_back_sm](https://github.com/user-attachments/assets/9957e199-c3e7-448f-89f1-9d7890242c13)

The control board is driven by an Atmega328PB and uses a DS3231MZ RTC for accurate time keeping. A coin cell battery is used to keep track of time even when the clock is unpowered. The time is initially set when programming the clock, and can be changed in +/- 30 minute increments using the two buttons on the back.

![control_pcb_sm](https://github.com/user-attachments/assets/3bf7e5e5-2889-41c3-bf6c-f9f17c3dfab5)

It uses a multiplexing system which allows the 24 segments to be controlled with 10 connections. The rays of the clock are divided in 3 groups, with 4 rays each. Each group has a top and bottom ground connection connecting the rays (allowing the half hour segments to be individually controlled). The “similar” rays of each group (e.g., the first ray of each group, the second ray of each group, etc) have a shared 5v connection. So for example, to turn on the segment representing the first half hour of 12 o’clock, ground the bottom connection of the first group and apply 5v to the first ray.


<img width="773" alt="display_schematic" src="https://github.com/user-attachments/assets/cdec5b86-ee2b-4f8e-b347-f77e1ad86d16" />



For safety (since we’re applying heat to paper here), I used an eFuse to clamp the input voltage at 5v. There are also SMD fuses to prevent against excessive current going through the segments. 

In order to keep the rays at the proper temperature to activate the thermochromic paint (ideally around 90-95 F / 32-35 C), a temperature sensor reads the ambient temperature and will PWM the display to maintain a reasonably consistent temperature. This actually got a bit complicated, since the heat from the display would warm up the back compartment where the control PCB is and influence the temperature sensor. Fortunately, the temperature increases were fairly predicable and could be compensated for. From testing, the clock works well in room temperatures between 65-80 F / 18-27 C.

![back_sm](https://github.com/user-attachments/assets/dd411040-db6b-44d3-813e-4c0d1ca0a4c2)
![front_side_sm](https://github.com/user-attachments/assets/0ddcf3a1-620f-4cdc-81a5-1680d852788e)
![side_sm](https://github.com/user-attachments/assets/40d22cb0-4554-4d00-bf48-511647fa9beb)

# Videos

[Startup sequence](https://www.youtube.com/watch?v=sf5E_oBU3c0)

[5 minutes of Sparkle Mode in real time](https://www.youtube.com/watch?v=p_eX9BhE4EM)

