/*
  ==============================================================================

    WaveGen.h
    Created: 17 May 2019 1:28:04am
    Author:  Matthew Martori

  ==============================================================================
*/

#pragma once

class WaveGenerator {
public:
    
    AudioSampleBuffer createHarmSineWavetable(int tableLength) {
        AudioSampleBuffer table;
        
        int harmonics[8] = { 1, 3, 5, 6, 7, 9, 13, 15 };
        float harmonicWeights[8] = { 0.5f, 0.1f, 0.05f, 0.0125f, 0.09f, 0.05f, 0.002f, 0.001f }; // [1]
        
        jassert (numElementsInArray (harmonics) == numElementsInArray (harmonicWeights));

        table.setSize(1, tableLength);
        auto * samples = table.getWritePointer(0); // [3]
        
        for (auto harmonic = 0; harmonic < numElementsInArray (harmonics); ++harmonic)
        {
            auto angleDelta = MathConstants<double>::twoPi / (double) (tableLength - 1) * harmonics[harmonic]; // [2]
            auto currentAngle = 0.0;
            for (auto i = 0; i < tableLength; ++i)
            {
                auto sample = std::sin (currentAngle);
                samples[i] += (float) sample * harmonicWeights[harmonic];                      // [3]
                currentAngle += angleDelta;
            }
        }
        
        samples[tableLength] = samples[0];
        
        return table;
    }
    
    AudioSampleBuffer createSineWavetable(int tableLength) {
        AudioSampleBuffer table;
        
        table.setSize(1, tableLength);
        auto * samples = table.getWritePointer(0); // [3]
    
        auto angleDelta = MathConstants<double>::twoPi / (double) (tableLength - 1); // [2]
        auto currentAngle = 0.0;
        
        for (auto i = 0; i < tableLength; ++i)
        {
            auto sample = std::sin (currentAngle*2);
            samples[i] += (float) sample;                      // [3]
            currentAngle += angleDelta;
        }
        
        samples[tableLength] = samples[0];
        
        return table;
    }
    
    AudioSampleBuffer createSawWavetable(int tableLength) {
        AudioSampleBuffer table;
        
        table.setSize(1, tableLength);
        auto * samples = table.getWritePointer(0); // [3]
        auto stepSize = 2 / (double)(tableLength - 1); // [4]
        for (auto i = 0; i < tableLength; ++i) {
            auto sample = -(i * stepSize) + 1; // [5]
            samples[i] = (float) sample;
        }
        
        return table;
    }
    
    
    
};
