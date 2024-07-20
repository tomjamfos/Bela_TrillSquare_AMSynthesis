/*
	Based on: https://www.youtube.com/watch?v=fufNzqgjej0&list=PLCrgFeG6pwQmdbB6l3ehC8oBBZbatVoz3&index=5&ab_channel=BelaPlatform
*/

#include "Wavetable.h"

Wavetable::Wavetable(float sampleRate, std::vector<float>& table, bool useInterp) {
	
	setup(sampleRate, table, useInterp);
	
}

void Wavetable::setup(float sampleRate, std::vector<float>& table, bool useInterp) {
	
	_inverseSampleRate = 1.0 / sampleRate;
	_table = table;
	_useInterp = useInterp;
	_readPointer = 0;
	
}

float Wavetable::process() {
	
	float out = 0;
	
	//if table is empty, return 0
	if(!_table.size()) {return out;}
	
	//advance read pointer according to frequency 
	_readPointer += _table.size() * _freq * _inverseSampleRate;
	//wrap read pointer back around
	while(_readPointer >= _table.size()) {_readPointer -= _table.size();}
	
	if(_useInterp) {
		//perform linear interpolation
		int indexBelow = _readPointer;
		int indexAbove = indexBelow + 1;
		if(indexAbove >= _table.size()) {indexAbove = 0;}
		float fractAbove = _readPointer - indexBelow;
		float fractBelow = 1.0 - fractAbove;
		out = fractBelow * _table[indexBelow] + fractAbove * _table[indexAbove];
	}
	else {
		//no interpolation
		out = _table[(int) _readPointer];
	}
	
	return out;
	
}