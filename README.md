# ClarinetMIDI

This repository contains the code to run a Clarinet MIDI controller. The hardware is based on a Teensy 2.0 MCU, wired to two MPR121 capacitive touch breakout boards from Adafruit. Each pad on the breakout is wired to a fingerpad (I used washers hot-glued to the PVC body). Breath is sensed with a MXP5010 air pressure sensor. 

The notes are contained in a lookup table, as a MIDI note number and an array of keys that are pressed for that note. After the note is looked up, it is shifted two octaves due to how computers interpret the MIDI notes.

All of the parts totaled approximately $100:
1x Teensy 2.0 (could probably use other Teensys, but the 2.0 is the cheapest)
2x MPR 121
1x MXP 5010
17 1" fender washers (keys)
17 #4 sheet metal screws (secure mounting of keys)
2' length of 1-1/2 PVC pipe
2x 1-1/2 PVC end caps
1x 2' of 1/4" ID Vinyl tubing for connecting mouthpiece to sensor. Poke a hole to let the air out
1x USB Female USB-B to Male microUSB (optional, makes the cable detachable)
1x 10' USB Male A-B cable
Laser cut plywood internal assembly (super-duper fancy shmancy)
2.5" feet of 28 AWG (17 strands wide) for hooking up pads
Hot glue/wire/solder/0.1" header pins and crimps/etc for connecting everything

