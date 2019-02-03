# 250-HW5

This is the 5th homework for 250. The idea was to take a previous homework and implement a different mapping while trying to use fauck.

To achieve this, I changed the layout of my previous instrument and moved all the audio generators to Faust while trying to maintain the same structure of oscilators


# Video Example
The following video is an example of the possibilities of this instrument

https://youtu.be/JkiGCthOR1M


# Chuck Side
Chuck is being used to route the MIDI signals as well as map them into the appropriate size to control faust parameters. In general, Chuck is being used as a host of a faust generator; however, it doesn't mean chuck is not doing a lot of work. All the mappings, preprocessing, control and final envelope are being controlled trhough chuck

# Faust Side
In this case, thanks to fauck, Im running faust with a sawtooth oscilator a pulse oscillator whose duty cycle is changing eriodically and a sine oscilator just for the very low frequency content. All of this parameters are smoothed through faust smoothers. Then the sum of the generators goes through a 4th order butterworth filter, that then goes through a third order distortion with an offset value, then through a DC Blocker and finally through a Moog VCF

# The Controller
The controller was built from scratch and assembled in a very nice transparent acrylic box which allows to dig into the details of the construction. Maybe I could add some encoders to the side as chaning parameters could be really helpful instead of goint through the code to do it!
