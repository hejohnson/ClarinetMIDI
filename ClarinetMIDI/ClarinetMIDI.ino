#include <Wire.h>
#include "Adafruit_MPR121.h"

#define NUM_NOTES 42
#define BREATHSENSEPIN 10

//#define USESERIAL

Adafruit_MPR121 buttons1 = Adafruit_MPR121();
Adafruit_MPR121 buttons2 = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t currTouched_buttons1 = 0;
uint16_t currTouched_buttons2 = 0;

uint32_t lastTouched = 0;
uint32_t currTouched = 0;

uint8_t currentMIDINote = 0;
uint8_t newMIDINote = 0;

union {
  struct {
    uint32_t padding:15;
    uint8_t pad17:1;
    uint8_t pad16:1;
    uint8_t pad15:1;
    uint8_t pad14:1;
    uint8_t pad13:1;
    uint8_t pad12:1;
    uint8_t pad11:1;
    uint8_t pad10:1;
    uint8_t pad9:1;
    uint8_t pad8:1;
    uint8_t pad7:1;
    uint8_t pad6:1;
    uint8_t pad5:1;
    uint8_t pad4:1;
    uint8_t pad3:1;
    uint8_t pad2:1;
    uint8_t pad1:1;  
  }
}

typedef struct {
  uint8_t midiNote;
  uint32_t keyMask;
} note_t;

// the MIDI channel number to send messages
const int channel = 1;

note_t noteLookupTable[NUM_NOTES] = {
  {58, 0b00000000000000010000000000011100}, // A#2
  {70, 0b00000000000000011100011100011100}, // A#3
  {82, 0b00000000000000011000000000000000}, // A#4
  {57, 0b00000000000000010000001000011100}, // A2
  {69, 0b00000000000000011010011100011100}, // A3
  {81, 0b00000000000000011000000000000100}, // A4
  {93, 0b00000000000000010000000000111100}, // B2
  {71, 0b00000000000000011000111100011100}, // B3
  {83, 0b00000000000000011000001100001100}, // B4
  {61, 0b00000000000000010000000001001100}, // C#3
  {73, 0b00000000000000011000101110011100}, // C#4
  {85, 0b00000000000000011000000000011000}, // C#5
  {60, 0b00000000000000010000000000001100}, // C3
  {72, 0b00000000000000011000011100011100}, // C4
  {84, 0b00000000000000011001000100011000}, // C5
  {51, 0b00000000000000010100011100011100}, // D#2
  {63, 0b00000000000000010000000000000000}, // D#3
  {75, 0b00000000000000011000000010011100}, // D#4
  {87, 0b00000000000000011001000000111000}, // D#5
  {50, 0b00000000000000010010011100011100}, // D2
  {62, 0b00000000000000010000000000000100}, // D3
  {74, 0b00000000000000011000001100011100}, // D4
  {86, 0b00000000000000011000100000011000}, // D5
  {52, 0b00000000000000010000111100011100}, // E2
  {64, 0b00000000000000000000000000000100}, // E3
  {76, 0b00000000000000011000001000011100}, // E4
  {88, 0b00000000000000011001000000001000}, // E5
  {54, 0b00000000000000010001011100011100}, // F#2
  {66, 0b00000000000000000000000000000010}, // F#3
  {78, 0b00000000000000011000000000111100}, // F#4
  {90, 0b00000000000000011001000100000000}, // F#5
  {53, 0b00000000000000010000011100011100}, // F2
  {65, 0b00000000000000000000000000000000}, // F3
  {77, 0b00000000000000011000000000011100}, // F4
  {89, 0b00000000000000011001001100010000}, // F5
  {56, 0b00000000000000010000000100011100}, // G#2
  {68, 0b00000000000000001000000000000001}, // G#3
  {80, 0b00000000000000011000000001001100}, // G#4
  {55, 0b00000000000000010000001100011100}, // G2
  {67, 0b00000000000000000000000000000001}, // G3
  {79, 0b00000000000000011000000000001100}, // G4
  {91, 0b00000000000000011000000010011000}, // G5
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

  #ifdef USESERIAL
  Serial.println("startup successful");
  #endif
}

void loop() {
  currTouched_buttons1 = buttons1.touched();
  currTouched_buttons2 = buttons2.touched();

  currTouched = (uint32_t)currTouched_buttons1 + ((uint32_t)currTouched_buttons2 << 12);
  
  if (analogRead(BREATHSENSEPIN) < 100) {
    #ifndef USESERIAL
    usbMIDI.sendNoteOff(currentMIDINote, 0, channel); // if you're not blowing, stop sending the note
    #endif
    currentMIDINote = 0;
  } else {
//    if (lastTouched != currTouched) {
//      #ifdef USESERIAL
//      Serial.println(currTouched, BIN);
//      #endif
      for (int i = 0; i < NUM_NOTES; i++) {
        if (currTouched == noteLookupTable[i].keyMask) { // Does the current index match an actual note?
          #ifdef USESERIAL
          Serial.print(currentMIDINote);
          Serial.println(" off");
          #else

          newMIDINote = noteLookupTable[i].midiNote; // figure out the matching MIDI note ID

          if (newMIDINote != currentMIDINote) { // If its a different note, stop sending the current note and send the new one
            usbMIDI.sendNoteOff(currentMIDINote, 0, channel); // stop sending the last note
            #endif
            currentMIDINote = newMIDINote;
            #ifdef USESERIAL
            Serial.print(currentMIDINote);
            Serial.println(" on");
            #else
            usbMIDI.sendNoteOn(currentMIDINote, 99, channel); // send the new note
            #endif
          }
          break; // stop looking for notes
        }
      }
      // reset our state
     // lastTouched = currTouched;
//    } else {
//      #ifndef USESERIAL
//      usbMIDI.sendNoteOff(currentMIDINote, 0, channel); //#TODO: not an actual note, not sending anything for now, but not sure if that's the desired behavior
//      #endif
//    }
  }

  // MIDI Controllers should discard incoming MIDI messages.
  // http://forum.pjrc.com/threads/24179-Teensy-3-Ableton-Analog-CC-causes-midi-crash
  #ifndef USESERIAL
  while (usbMIDI.read()) {
    // ignore incoming messages
  }
  #endif
  delay(50);
}
