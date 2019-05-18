/*
  ==============================================================================

    Voice.cpp
    Created: 18 May 2019 4:14:07am
    Author:  Matthew Martori

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "OscControls.h"
#include "OSC.h"
#include "WaveGen.h"

class Voice : public SynthesiserVoice {
public:
    Voice(OscControls& oc) : oscControls (oc){
        
        auto numberOfOscillators = 16;
        
        
        AudioSampleBuffer aTable = waveGen.createSineWavetable(unsigned(256));
        AudioSampleBuffer bTable = waveGen.createSawWavetable(unsigned(256));
        
        for (auto i = 0; i < numberOfOscillators; ++i) {
            auto * oscA = new WavetableOscillator(aTable);
            auto * oscB = new WavetableOscillator(bTable);
            
            a_oscillators.add(oscA);
            b_oscillators.add(oscB);
        }
        
    }
    
    void setADSR (ADSR::Parameters params) {
        adsr.setParameters(params);
    }
    
    
    bool canPlaySound (SynthesiserSound * sound) override {
        return true;
    }
    
    void pitchWheelMoved (int newPitchWheelValue) override{
        
    }
    
    void controllerMoved (int controllerNumber, int newControllerValue) override{
        
    }
    
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition) override {
        
        
        
    }
    
    void stopNote (float velocity, bool allowTailOff) override {
        
        
    }
    
    void renderNextBlock (AudioBuffer< float > &outputBuffer, int startSample, int numSamples) override {
        OscControls::oscParams params = oscControls.getParameters();
        
        //        double frequency = params.frequency;
        double balance = params.balance;
        double detuneGap = params.detuneGap;
        double numOsc = params.numOsc;
        
        double level = 0.25f / numOsc;
        
        
        
        auto * leftBuffer = outputBuffer.getWritePointer(0, startSample);
        auto * rightBuffer = outputBuffer.getWritePointer(1, startSample);
        
        
        outputBuffer.clear();
        
        for (auto oscillatorIndex = 0; oscillatorIndex < numOsc; ++oscillatorIndex) {
            auto * oscA = a_oscillators.getUnchecked(oscillatorIndex);
            auto * oscB = b_oscillators.getUnchecked(oscillatorIndex);
            
            double gapToUse = detuneGap;
            double rightLevel = .75;
            double leftLevel = .75;
            
            if (oscillatorIndex == 0) {
                rightLevel = 1.0;
                leftLevel = 1.0;
            } else if (oscillatorIndex % 2 == 0) {
                gapToUse *= -1;
                
                rightLevel = 0.75;
                leftLevel = 1.0;
            } else {
                rightLevel = 1.0;
                leftLevel = 0.75;
            }
            
            oscA -> setFrequency((float) frequency + (oscillatorIndex * gapToUse), currentSampleRate);
            oscB -> setFrequency((float) frequency + (oscillatorIndex * gapToUse), currentSampleRate);
            
            for (auto sample = 0; sample < outputBuffer.getNumSamples(); ++sample) {
                auto sineSample = oscA -> getNextSample();
                auto sawSample = oscB -> getNextSample();
                
                auto mix = (sineSample * balance) + (sawSample * (1.0 - balance));
                
                float levelSample = mix * level;
                
//                levelSample *= adsr.getNextSample();
                levelSample *= 1.0;
                
                leftBuffer[sample] += levelSample * leftLevel;
                rightBuffer[sample] += levelSample * rightLevel;
            }
        }
    }
private:
    ADSR adsr;
    OscControls& oscControls;
    double currentSampleRate = 44000;
    
    double frequency = 440.0;
    
    WaveGenerator waveGen;
    
    OwnedArray < WavetableOscillator > a_oscillators;
    OwnedArray < WavetableOscillator > b_oscillators;
};
