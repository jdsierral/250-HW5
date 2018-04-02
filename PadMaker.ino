#include <ResponsiveAnalogRead.h>
#include <Bounce.h>
#include <DSP.h>

//  ===================================  //
//  Description of Analog Inputs
//
//  A0 => Pitch Slider 1
//  A1 => Pitch Slider 2
//  A3 => Joystick 1.x
//  A4 => Joystick 1.y
//  A5 => Joystick 2.x
//  A6 => Joystick 2.y
//  A7 => Envelope Slider
//  ===================================  //
//  ===================================  //
//  Description of Digital Inputs
//
//  16 => Joystick Button 1
//  19 => Joystick Button 2
//  ===================================  //


#define BOUNCE_TIME 5


const int chan[] = {0, 1};
const int vel = 100;
const int A_PINS = 7;
const int D_PINS = 2;
const int A_IN[A_PINS] = {A0, A1, A3, A6, A7, A4, A8};
const int D_IN[D_PINS] = {16, 19};

int note [] = {60, 72};

byte data[A_PINS];
byte lag[A_PINS];

unsigned long prevMicros = 0;
unsigned long interval = 1000;

DiffPole* diffTracker;

ResponsiveAnalogRead analog[] {
    {A_IN[0], true},
    {A_IN[1], true},
    {A_IN[2], true},
    {A_IN[3], true},
    {A_IN[4], true},
    {A_IN[5], true},
    {A_IN[6], true}
}

Bounce buttons[] = {
    Bounce(D_IN[0], BOUNCE_TIME),
    Bounce(D_IN[1], BOUNCE_TIME)
}


void setup() {
    for (int i = 0; i < D_PINS; i++) {
        pinMode(D_IN[i], INPUT_PULLUP);
    }

    oneDiff = new OneDiff(1000);
}

void loop() {
    unsigned long startMicros = micros();
    getData();
    long diff = micros() - startMicros;
    if (interval - diff > 0) {
        delayMicroseconds( interval - diff );
    }
}

void getData() {
    getDigitalData();
    getAnalogData();
    while(usbMIDI.read()) {}
}

void getDigitalData() {
    for (int i = 0; i < D_PINS; i++) {
        digital[i].update();
        if (digital[i].fallingEdge()) {
            usbMIDI.sendNoteOn(note[i], vel, chan[i]);
        }
        if (digital[i].risingEdge()) {
            usbMIDI.sendNoteOff(note[i], vel, chan[i]);
        }
    }
}

void getAnalogData() {
    for (int i = 0; i < A_PINS; i++) {
        analog[i].update();
        if (i == 6) {
            getEnvelopeData();
            continue;
        }
        if(analog[i].hasChanged()) {
            data[i] = analog[i].getValue() >> 3;
            if (data[i] != lag[i]) {
                lag[i] = data[i];
                if (i < 4) {
                    usbMIDI.sendControlChange(cc[i], data[i], chan[ i % 2 ]);
                }  else {
                    usbMIDI.send
                }
            }
        }

    }

    for (int i = 0; i < A_PINS; i++) {
        analog[i].update();

        if (i >= 5) {
            continue;
        }
        if (analog[i].hasChanged()) {
            data[i] = analog[i].getValue();
            if (data[i] != lag[i]) {
                lag[i] = data[i];
                if (i < 4)
                    usbMIDI.sendControlChange(CCID[i], data[i] >> 3, CHAN);
                else
                    usbMIDI.setPitchBend(data[i], chan[i % 2]);
            }
        }
    }
}
