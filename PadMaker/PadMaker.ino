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


const int chan[] = {1, 2};
const int cc[] = {20, 22, 21, 23};
const int vel = 100;
const int A_PINS = 7;
const int D_PINS = 2;
const int A_IN[A_PINS] = {A0, A3, A1, A4, A7, A6, A8};
const int D_IN[D_PINS] = {16, 19};

int note [] = {60, 72};

byte data[4];
byte lag[4];
int dataS[3];
int lagS[3];

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
};

Bounce digital[] = {
    Bounce(D_IN[0], BOUNCE_TIME),
    Bounce(D_IN[1], BOUNCE_TIME)
};


void setup() {
    for (int i = 0; i < D_PINS; i++) {
        pinMode(D_IN[i], INPUT_PULLUP);
    }
    diffTracker = new DiffPole(1000);
    diffTracker->setTau(1);
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
    while (usbMIDI.read()) {}
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
        } else {
            continue;
        }

        if (analog[i].hasChanged()) {
            if (i < 4) {
                data[i] = analog[i].getValue() >> 3;
                if (data[i] != lag[i]) {
                    lag[i] = data[i];
                    usbMIDI.sendControlChange(cc[i], data[i], chan[i % 2]);
                }
            } else {
                int j = i - 4;
                dataS[j] = (analog[i].getValue() << 4) - 8192;
                if (dataS[j] != lagS[j]) {
                    lagS[j] = dataS[j];
                    usbMIDI.sendPitchBend(dataS[j], chan[j]);
                }
            }
        }
    }
}

void getEnvelopeData() {
    float val = analog[6].getValue() >> 3;
    float diff = diffTracker->tick(val);
    usbMIDI.sendControlChange(7, int(diff), chan[0]);
}
