import("stdfaust.lib");

note = hslider("Note",60,1,88,1) : si.smooth(ba.tau2pole(0.03));
freq = ba.midikey2hz(note);
Q = 10;
gain = 10.0;
filtFreq = hslider("filtFreq",1000,100,10000,0.01) : si.smoo;
filtRes = 0.5;
lfoFreq = hslider("LFOFreq",0,0.001,10,0.001) : si.smoo;
lfo = os.osc(lfoFreq);
duty = 0.25 + lfo * 0.2;
osc1 = os.sawtooth(freq * 3 / 2.0);
osc2 = os.pulsetrain(freq , duty);
osc3 = os.osc( freq * ( 1 + no.noise * 0.) / 2.0);

g1 = hslider("Gain1",0.73,0,1,0.01);
g2 = hslider("Gain2",0.55,0,1,0.01);
g3 = hslider("Gain3",1,0,1,0.01);

drive = hslider("drive",0.2, 0,1,0.01) : si.smoo;
offset = hslider("offset",1,0,5,0.01) : si.smoo;

gen = osc1 * g1, osc2 * g2, osc3 * g3 :> ef.cubicnl(drive, offset) : fi.dcblocker : ve.moog_vcf_2b( filtRes, filtFreq );

voice = gen;
// process = voice;
