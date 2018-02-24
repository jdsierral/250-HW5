Faust fck => dac;

fck.eval(`import("test.dsp"); process = test;`);


while(true) {
    1::samp => now;
}
