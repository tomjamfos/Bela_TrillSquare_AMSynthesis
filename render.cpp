/*
	Using Trill square to control the frequencies of carrier & 
	modulator oscillators in amplitude modulation.
	
	When a touch is recieved, the XY locations are stored untill
	a new touch location is read. These values are smoothed using
	a low-pass filter to avoid noisey values and sudden clicks when
	a touch is initially made.
	
	3 LEDs are used, 1 lights when a touch (above a small threshold)
	is read. The other 2 display a range of light intensity according
	to the location of the touch on the sensor.
*/

#include <Bela.h>
#include <cmath>
#include <vector>
#include <libraries/Trill/Trill.h>
#include <libraries/OnePole/OnePole.h>
#include <libraries/Gui/Gui.h>
#include <libraries/GuiController/GuiController.h>
#include <libraries/Scope/Scope.h>
#include "Wavetable.h"

//Gui objects declarations
Gui gGui;
GuiController gController;

//output led pins
const unsigned int kDigLedPin = 0;
const unsigned int kAnaLedPins[2] = {0, 1};

//Trill object declaration
Trill touchSensor;

//variable to store size of touch
float gTouchSize = 0;

//variables to store XY locations of touch
float gTouchLocations[2] = {0, 0};

//variables to update XY locations when touch is sensed
float gXYValues[2] = {.5, .5};

//Onepole object declarations
OnePole gSmoothingFilters[2];

//Wavetable object declarations
Wavetable gOscillators[2];

//Scope object declaration
Scope scope;

void loop(void*) {
	
	while(!Bela_stopRequested()) {
		//read sensor
		touchSensor.readI2C();
		//touch size
		gTouchSize = touchSensor.compoundTouchSize();
		//touch locations - adjusted in accordance with the position of Trill square (see diagram!)
		gTouchLocations[0] = 1.0 - touchSensor.compoundTouchLocation();
		gTouchLocations[1] = touchSensor.compoundTouchHorizontalLocation();
		//wait before next reading
		usleep(12000);
	}
	
}

bool setup(BelaContext *context, void *userData) {
	
	//setup gui
	gGui.setup(context->projectName);
	gController.setup(&gGui, "Trill - Amp Mod");
	gController.addSlider("dB", -6, -60, 0);
	
	//setup led
	pinMode(context, 0, kDigLedPin, OUTPUT);
	
	//setup trill
	if(touchSensor.setup(1, Trill::SQUARE) != 0) {
		fprintf(stderr, "Unable to initalise\n");
		return false;
	}
	Bela_runAuxiliaryTask(loop);
	
	//setup smoothing filters
	for(unsigned int i = 0; i < 2; i++) {
		OnePole filter(18, context->audioSampleRate);
		gSmoothingFilters[i] = filter;
	}
	
	//setup oscillators
	const unsigned int wavetableSize = 1024;
	std::vector<float> wavetable;
	wavetable.resize(wavetableSize);
	for(unsigned int i = 0; i < wavetableSize; i++) {
		wavetable[i] = sinf(2.0 * M_PI * (float) i / (float) wavetableSize);
	}
	for(unsigned int i = 0; i < 2; i++) {
		Wavetable osc(context->audioSampleRate, wavetable);
		gOscillators[i] = osc;
	}
	
	//setup scope
	scope.setup(3, context->audioSampleRate);
	
	return true;
	
}


void render(BelaContext *context, void *userData) {
	
	//set the amplitude of the synth according to gui slider
	float dB = gController.getSliderValue(0);
	float amp = powf(10.0, dB / 20.0);
	
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		
		//check for touches greater than a small threshold
		if(gTouchSize > .05) {
			//update XY values
			for(unsigned int i = 0; i < 2; i++) {
				gXYValues[i] = gTouchLocations[i];
			}
			//turn on led
			digitalWriteOnce(context, n, kDigLedPin, HIGH);
		}
		else {
			//turn off led
			digitalWriteOnce(context, n, kDigLedPin, LOW);
		}
		
		for(unsigned int i = 0; i < 2; i++) {
			//convert raw sensor value to smoothed
			float smoothedValue = gSmoothingFilters[i].process(gXYValues[i]);
			//convert value to frequency range between 55 & 880
			float freq = map(smoothedValue, 0, 1, 55, 880);
			//set oscillator frequencies 
			gOscillators[i].setFreq(freq);
			//convert value to range between 0.3 (off) & 1 (fully on) for leds
			float ledValue = map(smoothedValue, 0, 1, .3, 1);
			//write converted value to leds
			analogWrite(context, n, kAnaLedPins[i], ledValue);
		}
		
		//amplitude modulation 
		float out = gOscillators[0].process() * (gOscillators[1].process() * .5 + .5);
		
		//scale output by amplitude
		out *= amp;
		
		//log output to scope
		scope.log(out, gXYValues[0], gXYValues[1]);
		
		//reduce maximum output level
		out *= .5;
		
		//send to audio out
		for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
			audioWrite(context, n, channel, out);
		}
		
	}
	
}

void cleanup(BelaContext *context, void *userData) {
	
}
