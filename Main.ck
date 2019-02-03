Gain out;
out => dac.chan(8);
out => dac.chan(7);

JCRev verb;

out => verb => LPF verbLp => dac;
verbLp.freq(3500);
verb.mix(0.1);


1 => int DEBUG;

1::second / 1::samp => float fs;

Faust fck[2];
Mapping map[2];
VelTracker velTracker[2];
ComplexTracker envTracker[2];
[0.0, 0.0] @=> float rawEnv[];
[0, 0] @=> int state[];
Gain env[2];

for (0 => int i; i < 2; i++) {
    fck[i] => env[i] => out;
    fck[i].eval(`import("Voice.dsp"); process = voice;`);
    envTracker[i].setFs(fs);
    envTracker[i].setTauAttack( 20.0 );
    envTracker[i].setTauRelease( 1.0 );
    velTracker[i].setFs(fs);
    velTracker[i].setTau( 600 );
    0 => map[i].base;
    16 => map[i].numNotes;
    4 => map[i].octave;
}
2 => map[0].mapping;
9 => map[1].mapping;
-7 => map[0].transpose;
 0 => map[1].transpose;

 out.gain(0.5);


spork ~ activateMIDI();
spork ~ sampleEnvelopeData();

fun void activateMIDI() {
    MidiIn midiIn;
    MidiMsg msg;

    midiIn.open("Teensy MIDI");

    <<< midiIn.name() >>>;
    <<< "Loaded" >>>;

    0 => int slider3Lag;

    while ( true ) {
        midiIn => now;
        while(midiIn.recv(msg)) {
            /* <<< msg.data1, msg.data2, msg.data3 >>>; */
            if (msg.data2 == 21) {
                updateJLY(msg.data3);
            } else if (msg.data2 == 22) {
                updateJLX(msg.data3);
            } else if (msg.data2 == 23) {
                updateJRY(msg.data3);
            } else if (msg.data2 == 24) {
            } else if (msg.data2 == 25) {
                if (msg.data3 > 2) updateSlider(0, msg.data3);
            } else if (msg.data2 == 26) {
                if (msg.data3 > 2) updateSlider(1, msg.data3);
            } else if (msg.data2 == 27) {
                if (Std.fabs(msg.data3 - slider3Lag) < 50) {
                    msg.data3 => int val;
                    updateSlider3(val);
                    val => slider3Lag;
                }
            } else if (msg.data1 == 143 && msg.data2 == 60) {
                switchState1();
            } else if (msg.data1 == 143 && msg.data2 == 72) {
                switchState2();
            }
        }
    }
}

fun void updateJLY(int val){
    val / 127.0 * 100.0 => rawEnv[0];
}

fun void updateJLX(int val){}

fun void updateJRY(int val){
    val / 127.0 * 100.0 => rawEnv[1];
}

fun void updateSlider(int freqNum, int val){
    (val/127.0) => float pos;
    map[freqNum].map(pos) => float note;
    fck[freqNum].v( "Note", note);
    printFaust(freqNum);
}

fun void updateSlider3(int val){
    if (val > 10) {
        if (state[0] == 0 && state[1] == 0) {
            val / 127.0 * 5 => float pos;
            for (0 => int i; i < 2; i++) {
                fck[i].v("LFOFreq", pos);
            }
        } else if (state[0] == 0 && state[1] == 1) {
            val / 127.0 * 36 => float note;
            Std.mtof(note + 80) => float freq;
            for(0 => int i; i < 2; i++) {
                fck[i].v("filtFreq", freq);
            }
        } else if (state[0] == 1 && state[1] == 0) {
            val / 127.0 => float drive;
            for(0 => int i; i < 2; i++) {
                fck[i].v("drive", drive);
            }
        } else {
            val / 127.0 * 5 => float offset;
            for(0 => int i; i < 2; i++) {
                fck[i].v("offset", offset);
            }
        }
    }
    printFaust(0);
}

fun void switchState1(){
    state[0] == 0 => state[0];
    printState();
}
fun void switchState2(){
    state[1] == 0 => state[1];
    printState();
}

fun void sampleEnvelopeData() {
    while(true) {
        velTracker[0].setTarget( rawEnv[0] );
        velTracker[1].setTarget( rawEnv[1] );
        20::ms => now;
    }
}

fun void printState() {
    if (DEBUG == 1) {
        if (state[0] == 0 && state[1] == 0) {
            <<< "LFO Freq" >>>;
        } else if (state[0] == 0 && state[1] == 1) {
            <<< "Filt Freq" >>>;
        } else if (state[0] == 1 && state[1] == 0) {
            <<< "Drive" >>>;
        } else {
            <<< "Offset" >>>;
        }
    }
}


fun void printFaust(int num) {
    if (DEBUG == 1)  fck[num].dump();
}

fun void printVal(float val) {
    if (DEBUG == 1) <<< val >>>;
}


while(true) {
    for (0 => int i; i < 2; i++) {
        velTracker[i].tick() => float envVal;
        envTracker[i].tick(envVal) => env[i].gain;
    }
    1::samp => now;
}
