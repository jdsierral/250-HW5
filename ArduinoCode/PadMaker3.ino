//
//  Arduino.cpp
//  DSP
//
//  Created by Juan David Sierra on 3/26/18.
//  Copyright © 2018 Juan David Sierra. All rights reserved.
//

#include "Arduino.hpp"


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

const int note = {60, 61};
const int chan = {0, 1};// MIDI Channel
const int vel = 100;    // Velocity

const int APINS = 6;    // Number of Analog Pins
const int JPINS = 3;    // Number of Joystick Pins
const int SPINS = 3;    // Number of Slider Pins
const int DPINS = 2;    // Number of Buttons

const int J_IN[JPINS] = {A3, A6, A7};
const int S_IN[SPINS] = {A0, A1, A8};
const int D_IN[DPINS] = {16, 19};

const int BOUNCE_TIME = 5;

byte jData[JPINS];
byte sData[SPINS];
byte dData[DPINS];

byte jLag [JPINS];
byte sLag [SPINS];
byte dLag [DPINS];

unsigned long prevUs = 0;
unsigned long uPeriod = 1000;

ResponsiveAnalogRead joysticks[] = {
    {J_IN[0], true},
    {J_IN[1], true},
    {J_IN[2], true},
    {J_IN[0], true}//,
};

ResponsiveAnalogRead sliders[] = {
    {J_IN[0], true},
    {J_IN[1], true},
    {J_IN[2], true}
};

Bounce buttons[] = {
    Bounce(D_IN[0], BOUNCE_TIME),
    Bounce(D_IN[1], BOUNCE_TIME),
};

void setup() {
    for (int i = 0; i < D_PINS; i++) {
        pinMode(D_IN[i], INPUT_PULLUP);
    }
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
    getButtonsData();
    getJoystickData();
    getPitchData();
    getEnvelopeData();
    while (usbMIDI.read()) {}
}

void getButtonsData() {
    for (int i = 0; i < D_PINS; i++) {
        digital[i].update();
        if(digital[i].fallingEdge()) {
            usbMIDI.sendNoteOn(note[i], vel, chan[i]);
        }
        if (digital[i].risingEdge()) {
            usbMIDI.sendNoteOff(note[i], vel, chan[i]);
        }
    }
}

void getJoystickData() {
    for (int i = 0; i < JPINS; i++) {
        joysticks[i].update();
        if (joysticks[i].hasChanged()) {
            data[i] = analog[i].getValue() >> 3;
            if (jdata[i] != jlag[i]) {
                jlag[i] = jdata[i];
                usbMIDI.sendControlChange(cc[i], jdata[i], chan[ i < 2 ?? 0 : 1 ]);
            }
        }
    }
}

void getPitchData() {
    for (int i = 0; i < 2; i++) {
        sliders[i].hasChanged()
    }
}

void getEnvelopeData() {

}
