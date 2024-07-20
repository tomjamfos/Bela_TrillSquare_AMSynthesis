/*
	Based on: https://www.youtube.com/watch?v=fufNzqgjej0&list=PLCrgFeG6pwQmdbB6l3ehC8oBBZbatVoz3&index=5&ab_channel=BelaPlatform
*/

#pragma once
#include <vector>

class Wavetable {
	
	public:
		
		Wavetable() {} 
		Wavetable(float sampleRate, std::vector<float>& table, bool useInterp = true);
		void setup(float sampleRate, std::vector<float>& table, bool useInterp = true);
		void setFreq(float freq) {_freq = freq;}
		float getFreq() {return _freq;}
		float process();
		~Wavetable() {} 
		
	private:
		
		float _inverseSampleRate;
		std::vector<float> _table;
		bool _useInterp;
		float _freq;
		float _readPointer;
	
};