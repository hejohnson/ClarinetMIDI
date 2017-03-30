#include <Wire.h>
#include "Adafruit_MPR121.h"

#define NUM_NOTES 43
#define BREATHSENSEPIN 10

#define BREATHRESTING_CUTOFF 55
#define BREATHMAX 512

//#define USESERIAL

Adafruit_MPR121 buttons1 = Adafruit_MPR121();
Adafruit_MPR121 buttons2 = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t currTouched_buttons1 = 0;
uint16_t currTouched_buttons2 = 0;

uint32_t currTouched = 0;

uint8_t currentMIDINote = 0;
uint8_t newMIDINote = 0;

bool noteOffSent = false;

typedef struct {
  uint8_t midiNote;
  //uint8_t numberKeys
  int8_t keys[9];
} note_t;

// the MIDI channel number to send messages
const int channel = 1;

uint32_t genKeyMask(int8_t* key) {
  uint32_t blankMask = 0;
  for (int i = 0; i<9; i++) {
    if (key[i] == -1) {
      continue;
    } else {
      blankMask |= (1L<<key[i]);
    }
  }
  return blankMask;
}

note_t noteLookupTable[NUM_NOTES] = {
  {26,{16,2,3,4,8,9,10,13,-1}},
  {27,{16,2,3,4,8,9,10,14,-1}},
  {28,{16,2,3,4,8,9,10,11,-1}},
  {29,{16,2,3,4,8,9,10,-1,-1}},
  {30,{16,2,3,4,8,9,10,12,-1}},
  {31,{16,2,3,4,8,9,-1,-1,-1}},
  {32,{16,2,3,4,8,-1,-1,-1,-1}},
  {33,{16,2,3,4,9,-1,-1,-1,-1}},
  {34,{16,2,3,4,-1,-1,-1,-1,-1}},
  {35,{16,2,3,4,5,-1,-1,-1,-1}},
  {36,{16,2,3,-1,-1,-1,-1,-1,-1}},
  {37,{16,2,3,6,-1,-1,-1,-1,-1}},
  {38,{16,2,-1,-1,-1,-1,-1,-1,-1}},
  {39,{16,-1,-1,-1,-1,-1,-1,-1,-1}},
  {40,{2,-1,-1,-1,-1,-1,-1,-1,-1}},
  {41,{-1,-1,-1,-1,-1,-1,-1,-1,-1}},
  {42,{1,-1,-1,-1,-1,-1,-1,-1,-1}},
  {43,{0,-1,-1,-1,-1,-1,-1,-1,-1}},
  {44,{15,0,-1,-1,-1,-1,-1,-1,-1}},
  {45,{15,16,2,3,4,8,9,10,13}},
  {46,{15,16,2,3,4,8,9,10,14}},
  {46,{15,16,2,3,4,7,8,9,10}},
  {47,{15,16,2,3,4,8,9,10,11}},
  {48,{15,16,2,3,4,8,9,10,-1}},
  {49,{15,16,2,3,4,8,9,10,12}},
  {50,{15,16,2,3,4,8,9,-1,-1}},
  {51,{15,16,2,3,4,8,-1,-1,-1}},
  {52,{15,16,2,3,4,9,-1,-1,-1}},
  {53,{15,16,2,3,4,-1,-1,-1,-1}},
  {54,{15,16,2,3,4,5,-1,-1,-1}},
  {55,{15,16,2,3,-1,-1,-1,-1,-1}},
  {56,{15,16,2,3,6,-1,-1,-1,-1}},
  {57,{15,16,2,-1,-1,-1,-1,-1,-1}},
  {58,{15,16,-1,-1,-1,-1,-1,-1,-1}},
  {59,{15,16,3,4,8,9,-1,-1,-1}},
  {60,{15,16,3,4,8,12,-1,-1,-1}},
  {61,{15,16,3,4,9,12,-1,-1,-1}},
  {62,{15,16,3,4,12,-1,-1,-1,-1}},
  {63,{15,16,3,4,5,12,-1,-1,-1}},
  {64,{15,16,3,12,-1,-1,-1,-1,-1}},
  {65,{15,16,2,8,9,12,-1,-1,-1}},
  {66,{15,16,2,9,12,-1,-1,-1,-1}},
};

void setup() {
  #ifdef USESERIAL
  Serial.begin(9600);
  #endif

  if(!buttons1.begin(0x5A)) {
    #ifdef USESERIAL
    Serial.println("0x5A Failed to init");
    #endif
  }

  if (!buttons2.begin(0x5B)) {
    #ifdef USESERIAL
    Serial.println("0x5B Failed to init");
    #endif
  }

}

void loop() {
  currTouched_buttons1 = buttons1.touched();
  currTouched_buttons2 = buttons2.touched();

  currTouched = (uint32_t)currTouched_buttons1 + ((uint32_t)currTouched_buttons2 << 12);
  
  if (analogRead(BREATHSENSEPIN) < BREATHRESTING_CUTOFF) {
    if (!noteOffSent) {
      usbMIDI.sendNoteOff(currentMIDINote, 0, channel); // if you're not blowing, stop sending the note
      noteOffSent = true;
    }
  } else {
      for (int i = 0; i < NUM_NOTES; i++) {
        uint32_t keyMask = genKeyMask(noteLookupTable[i].keys);
        if (currTouched == keyMask) { // Does the current index match an actual note?
          newMIDINote = noteLookupTable[i].midiNote+24; // figure out the matching MIDI note ID

          if (newMIDINote != currentMIDINote || noteOffSent) { // If its a different note, stop sending the current note and send the new one
            noteOffSent = false;
            if (newMIDINote != currentMIDINote) {
              usbMIDI.sendNoteOff(currentMIDINote, 0, channel); // stop sending the last note
            }
            currentMIDINote = newMIDINote;
            usbMIDI.sendNoteOn(currentMIDINote, constrain(map(analogRead(BREATHSENSEPIN), BREATHRESTING_CUTOFF, BREATHMAX-25, 0, 99), 15, 99), channel); // send the new note
          }
          break; // stop looking for notes
        }
      }
  }

  // MIDI Controllers should discard incoming MIDI messages.
  // http://forum.pjrc.com/threads/24179-Teensy-3-Ableton-Analog-CC-causes-midi-crash
  while (usbMIDI.read()) {
    // ignore incoming messages
  }
  delay(50);
}
