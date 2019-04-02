# AudioSpectrumVisualizer
Audio Spectrum Visualizer with Arduino.
Displays Audio Frequency Bands with an LED matrix.

-------------------------------------------------
Audio signal from a standard AUX cable is converted from Stereo to Mono and then fed into an MSGEQ7 chip.
This divides the signal into seven frequency bands, though I only display the first six. Each "bar" of LEDs on the matrix lights up corresponding to the level outputted from the MSGEQ7. The noisy signal is initally read, and the output level is adjusted accordingly. 
It is then semi-filtered with a moving average to give it slighty smoother movement. Finally, it is mapped between moving max and min levels - with a reset every so often - to make it "look good" and consistently stay balanced within the matrix. AKA it shouldn't stay maxed out for long because it will adjust it self to the rising max.

The matrix was made in the an overly complicated way (for fun) by using six daisy-chained shift registers.
