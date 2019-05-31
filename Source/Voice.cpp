/*
  ==============================================================================

    Voice.cpp
    Created: 18 May 2019 4:14:07am
    Author:  Matthew Martori

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "OscControls.h"
#include "ADSRControls.cpp"
#include "OSC.h"
#include "WaveGen.h"
#include <math.h>




struct WaveTableSound   : public SynthesiserSound
{
    WaveTableSound() {}
    
    bool appliesToNote    (int) override        { return true; }
    bool appliesToChannel (int) override        { return true; }
};

class Voice : public SynthesiserVoice {
public:
    Voice(OscControls& oc, ADSRControls& adsrC, int index_) : oscControls (oc), adsrControls (adsrC),index(index_){
        
        auto numberOfOscillators = 16;
        
        
        aTable = waveGen.createSineWavetable(aTable, 512);
        
        std::cout <<  "\nosc# " << index << "  --------------------------------\n ";

        for (int i = 0; i < 512; i++) {
            std::cout << aTable.getSample(0,i) << " ";
        }
        
        for (auto i = 0; i < numberOfOscillators; ++i) {
            aTable = waveGen.createSineWavetable(aTable, 512);
            bTable = waveGen.createSawWavetable(bTable, 512);
            
            auto * oscA = new WavetableOscillator(aTable);
            auto * oscB = new WavetableOscillator(bTable);
            
            a_oscillators.add(oscA);
            b_oscillators.add(oscB);
        }
    
        adsr.setSampleRate(getSampleRate());
    }
    
    void setADSR (ADSR::Parameters params) {
        
        adsr.setParameters(params);
        
    }
    
    
    void setIndex (int idx) {
        
        index = idx;
    }
    
    
    bool canPlaySound (SynthesiserSound * sound) override {
        return true;
    }
    
    void pitchWheelMoved (int newPitchWheelValue) override{
        
    }
    
    void controllerMoved (int controllerNumber, int newControllerValue) override{
        
    }
    
    struct oscDetuneParams {
        double freq;
        double pan;
    };
    
    vector<double> pan(double panLevel) {
        vector<double> output;
        // pan[-1;+1]
        double leftOut  = min (1 - panLevel, 1.0);  // max gain = 1.0, pan = 0: left and right unchanged)
        double rightOut = min (1 + panLevel, 1.0);
        
        output.push_back(leftOut);
        output.push_back(rightOut);
        
        return output;
    }
    
    vector<oscDetuneParams> createDetuneArray(double numOsc, double freq, double detuneGap) {
        vector<oscDetuneParams> outputValues;
        
        if (numOsc == 1) {
            outputValues.push_back({.freq = freq, .pan = 0.0});
            return outputValues;
        }
        
        int isOdd = int(numOsc) % 2;
        double panStep = (2 / (numOsc-1));
        
        
        for (double i = 0; i < numOsc; i++) {
            double startOffset = ((numOsc - isOdd) / 2) * detuneGap;
            if (!isOdd) {
                startOffset -= (detuneGap * .5);
            }
            
            
            double p = (-1.0 + (i * panStep)) * detuneGap;
            
            double freqDelta = startOffset - (i * detuneGap);
            freqDelta = freqDelta * 2.0;
            
            
            double adjustedFreq = freq - freqDelta;
            
            outputValues.push_back({.freq=adjustedFreq, .pan=p});
            
        }
        
        return outputValues;
        
        
    };
    
    
    
    
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition) override {
        
        frequency = MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        std::cout << "Note!" << midiNoteNumber << " idx:" << index << adsr.getParameters().attack
        
        << "\n";
        
        adsr.noteOn();
        
    }
    
    void stopNote (float velocity, bool allowTailOff) override {
        std::cout << "Note!off"  << "\n";
//        clearCurrentNote();
        adsr.noteOff();
    }
    
    void renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override {
        
        if (isVoiceActive()) {
        OscControls::oscParams params = oscControls.getParameters();
        ADSR::Parameters adsrParams = adsrControls.getParameters();
            
        setADSR(adsrParams);
        
        double trim = params.frequency;
        double balance = params.balance;
        double detuneGap = params.detuneGap;
        double numOsc = params.numOsc;
        
        double level = 0.05f / (numOsc/8);
            
        vector<oscDetuneParams> detuneParams =  createDetuneArray(numOsc, frequency, detuneGap);
        
        
        AudioSampleBuffer tempOutput;
            
            
        tempOutput.setSize(2, numSamples + startSample);
            
        for (int channelIdx = 0; channelIdx <= 1; channelIdx++) {
                
            auto table = tempOutput.getWritePointer(channelIdx);
            for (int i = 0; i < tempOutput.getNumSamples(); i++) {
                table[i] = 0;
                table[i] = 0;
            }
        }
        
        
        
//        outputBuffer.clear();
        
        for (auto oscillatorIndex = 0; oscillatorIndex < numOsc; oscillatorIndex++) {
            auto * oscA = a_oscillators.getUnchecked(oscillatorIndex);
            auto * oscB = b_oscillators.getUnchecked(oscillatorIndex);
            
            
            
            oscDetuneParams settings = detuneParams.at(oscillatorIndex);
            vector<double> stereoLevels = pan(settings.pan);
            
            double leftLevel = stereoLevels.at(0);
            double rightLevel = stereoLevels.at(1);

            
            oscA -> setFrequency(settings.freq + trim, getSampleRate());
            oscB -> setFrequency(settings.freq + trim, getSampleRate());
            
            for (auto sample = 0; sample < numSamples; sample++) {
                auto sineSample = oscA -> getNextSample();
                auto sawSample = oscB -> getNextSample();
                
                auto mix = (sineSample * balance) + (sawSample * (1.0 - balance));
                
                float levelSample = mix * level;
                
                
                tempOutput.addSample(0, sample + startSample, levelSample * float(leftLevel));
                tempOutput.addSample(1, sample + startSample, levelSample * float(rightLevel));
            }
            
            
        }
        
        adsr.applyEnvelopeToBuffer(tempOutput, startSample, numSamples);
            
        
        
        outputBuffer.addFrom(0, startSample, tempOutput, 0, startSample, numSamples);
        outputBuffer.addFrom(1, startSample, tempOutput, 1, startSample, numSamples);
            
        }
        
        if (!adsr.isActive()) {
            clearCurrentNote();
        }
    }
private:
    ADSR adsr;
    OscControls& oscControls;
    ADSRControls& adsrControls;
    double currentSampleRate = 44000;
    
    double frequency = 440.0;
    double pitchBend = 440.0;
    
    int index;
    
    WaveGenerator waveGen;
    
    AudioSampleBuffer aTable;
    AudioSampleBuffer bTable;
    
    
    
    OwnedArray < WavetableOscillator > a_oscillators;
    OwnedArray < WavetableOscillator > b_oscillators;
};
