//
//  OSC.h
//  wavvvvvvtable
//
//  Created by Matthew Martori on 5/17/19.
//

#ifndef OSC_h
#define OSC_h



//==============================================================================
class WavetableOscillator {
public:
    WavetableOscillator(const AudioSampleBuffer & wavetableToUse): wavetable(wavetableToUse),
    tableSize(wavetable.getNumSamples() - 1) {
        jassert(wavetable.getNumChannels() == 1);
    }
    
    void setFrequency(float frequency, float sampleRate) {
        auto tableSizeOverSampleRate = tableSize / sampleRate;
        tableDelta = frequency * tableSizeOverSampleRate;
    }
    
    void reset() {
        currentIndex = 0.0f;
    }
    
    forcedinline float getNextSample() noexcept {
        auto tableSize = wavetable.getNumSamples();
        auto index0 = (unsigned int) currentIndex;
        auto index1 = index0 + 1;
        auto frac = currentIndex - (float) index0; // [7]
        auto * table = wavetable.getReadPointer(0); // [8]
        auto value0 = table[index0];
        auto value1 = table[index1];
        auto currentSample = value0 + frac * (value1 - value0); // [9]
        if ((currentIndex += tableDelta) > tableSize) // [10]
            currentIndex = 0;
        return currentSample;
    }
    
private:
    const AudioSampleBuffer & wavetable;
    const int tableSize;
    float currentIndex = 0.0f, tableDelta = 0.0f;
};



#pragma once



#endif /* OSC_h */
