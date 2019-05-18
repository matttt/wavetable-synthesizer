/*
 ==============================================================================
 
 This file is part of the JUCE tutorials.
 Copyright (c) 2017 - ROLI Ltd.
 
 The code included in this file is provided under the terms of the ISC license
 http://www.isc.org/downloads/software-support-policy/isc-license. Permission
 To use, copy, modify, and/or distribute this software for any purpose with or
 without fee is hereby granted provided that the above copyright notice and
 this permission notice appear in all copies.
 
 THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
 WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
 PURPOSE, ARE DISCLAIMED.
 
 ==============================================================================
 */

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.
 
 BEGIN_JUCE_PIP_METADATA
 
 name:             WavetableSynthTutorial
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Wavetable synthesiser.
 
 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
 juce_audio_processors, juce_audio_utils, juce_core,
 juce_data_structures, juce_events, juce_graphics,
 juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2017, linux_make
 
 type:             Component
 mainClass:        MainContentComponent
 
 useLocalCopy:     1
 
 END_JUCE_PIP_METADATA
 
 *******************************************************************************/

#include "OSC.h"
#include "WaveGen.h"
#include "WaveViz.h"
#include "ButtonListener.h"
#pragma once


class Visualiser: public AudioVisualiserComponent {
public: Visualiser(): AudioVisualiserComponent(1) {
    setBufferSize(256*64);
    setSamplesPerBlock(256);
    setColours(Colours::black, Colours::indianred);
}
    
    void setSPB(int val) {
        setBufferSize(val);
    }
};

//==============================================================================
class MainContentComponent: public AudioAppComponent,
public Timer {
public: MainContentComponent() {
    cpuUsageLabel.setText("CPU Usage", dontSendNotification);
    cpuUsageText.setJustificationType(Justification::right);
    addAndMakeVisible(cpuUsageLabel);
    addAndMakeVisible(cpuUsageText);
    
    
    addAndMakeVisible(waveViz);
    
    frequencyLabel.setJustificationType(Justification::left);
    frequencyLabel.setText("Frequency (Hz)", dontSendNotification);
    addAndMakeVisible(frequencyLabel);
    addAndMakeVisible(frequencySlider);
    frequencySlider.setRange(3.0, 5000.0);
    frequencySlider.setSkewFactorFromMidPoint(500.0); // [4]
    frequencySlider.onValueChange = [this] {
        updateFrequency();
    };
    
    balanceLabel.setJustificationType(Justification::left);
    balanceLabel.setText("WT Pos", dontSendNotification);
    addAndMakeVisible(balanceLabel);
    addAndMakeVisible(balanceSlider);
    balanceSlider.setRange(0.0, 1.0);
    //        balanceSlider.setSkewFactorFromMidPoint (.05); // [4]
    balanceSlider.onValueChange = [this] {
        updateBalance();
    };
    
    detuneLabel.setJustificationType(Justification::left);
    detuneLabel.setText("Detune (gap between osc in Hz)", dontSendNotification);
    addAndMakeVisible(detuneLabel);
    addAndMakeVisible(detuneSlider);
    detuneSlider.setRange(0.0, 1.0);
    //        balanceSlider.setSkewFactorFromMidPoint (.05); // [4]
    detuneSlider.onValueChange = [this] {
        updateDetune();
    };
    
    oscLabel.setJustificationType(Justification::left);
    oscLabel.setText("Number of Oscillators", dontSendNotification);
    addAndMakeVisible(oscLabel);
    addAndMakeVisible(oscSlider);
    oscSlider.setRange(1.0, 16.0, 1.0);
    //        balanceSlider.setSkewFactorFromMidPoint (.05); // [4]
    oscSlider.onValueChange = [this] {
        updateOscillators();
    };
    
    addAndMakeVisible(visualiser);
    
    addAndMakeVisible(triggerButton);
    triggerButton.setButtonText("Trigger");
    triggerButton.addMouseListener(&buttonListener, false);

    
    buttonListener.onMouseUp = [this](const MouseEvent& e) {
        adsr.noteOff();
    };
    
    buttonListener.onMouseDown = [this](const MouseEvent& e) {
        adsr.noteOn();
    };
    
    
    
    sineTable = waveGen.createSineWavetable(sineTable, tableSize);
    sawTable = waveGen.createSawWavetable(sawTable, tableSize);
    
    
    waveViz.setWaveTables(sineTable, sawTable);
    
    adsr.setParameters({
        .attack =  1.0f,
        .decay = 0.3f,
        .sustain = 1.0f,
        .release = 2.0f
    });
    
    setSize(400, 400);
    setAudioChannels(0, 2); // no inputs, two outputs
    startTimer(50);
}
    
    ~MainContentComponent() {
        shutdownAudio();
    }
    
//    bool keyStateChanged(bool keyState, Component *c) override {
//
//        if (keyState == true) {
//            adsr.noteOn();
//        } else {
//            adsr.noteOff();
//        }
//
//        return keyState;
//    }
//
    void resized() override {
        int const oscWidth = 400;
        
        
        cpuUsageLabel.setBounds(10, 10, oscWidth - 20, 20);
        cpuUsageText.setBounds(10, 10, oscWidth - 20, 20);
        
        waveViz.setBounds(10, 40, oscWidth - 20, 80);
        
        frequencyLabel.setBounds(10, 30 + 100, oscWidth - 20, 20);
        frequencySlider.setBounds(10, 50 + 100, oscWidth - 20, 20);
        
        balanceLabel.setBounds(10, 70 + 100, oscWidth - 20, 20);
        balanceSlider.setBounds(10, 90 + 100, oscWidth - 20, 20);
        
        detuneLabel.setBounds(10, 110 + 100, oscWidth - 20, 20);
        detuneSlider.setBounds(10, 130 + 100, oscWidth - 20, 20);
        
        oscLabel.setBounds(10, 150 + 100, oscWidth - 20, 20);
        oscSlider.setBounds(10, 170 + 100, oscWidth - 100, 20);
        
        triggerButton.setBounds(oscWidth - 80, 170 + 100, 70, 20);
        
        visualiser.setBounds(10, 200 + 100, oscWidth - 20, 80);
        
        
    }
    
    void timerCallback() override {
        auto cpu = deviceManager.getCpuUsage() * 100;
        cpuUsageText.setText(String(cpu, 6) + " %", dontSendNotification);
    }
    
    void updateFrequency() {
        frequency = frequencySlider.getValue(); // [3]
    }
    
    void updateBalance() {
        balance = balanceSlider.getValue(); // [3]
        
        int index = (int)round(balance * 1023.0);
                
        waveViz.setWTPos(index);
        
    }
    
    void updateDetune() {
        detuneGap = detuneSlider.getValue(); // [3]
    }
    
    void updateOscillators() {
        numOsc = (int) round(oscSlider.getValue());
        level = 0.25f / numOsc;
    }
    
    void prepareToPlay(int, double sampleRate) override {
        currentSampleRate = sampleRate;
        auto numberOfOscillators = 16;
        
        visualiser.clear();
        
        for (auto i = 0; i < numberOfOscillators; ++i) {
            auto * oscillatorSine = new WavetableOscillator(sineTable);
            auto * oscillatorSaw = new WavetableOscillator(sawTable);
            //            auto midiNote = Random::getSystemRandom().nextDouble() * 36.0 + 48.0;
            
            sineoscillators.add(oscillatorSine);
            sawoscillators.add(oscillatorSaw);
        }
        
//        level = 0.25f / numberOfOscillators;
    }
    
    void releaseResources() override {}
    
    void getNextAudioBlock(const AudioSourceChannelInfo & bufferToFill) override {
        visualiser.setSPB(bufferToFill.numSamples);
        
        auto * leftBuffer = bufferToFill.buffer -> getWritePointer(0, bufferToFill.startSample);
        auto * rightBuffer = bufferToFill.buffer -> getWritePointer(1, bufferToFill.startSample);
        
        
        bufferToFill.clearActiveBufferRegion();
        
        for (auto oscillatorIndex = 0; oscillatorIndex < numOsc; ++oscillatorIndex) {
            auto * oscillatorSine = sineoscillators.getUnchecked(oscillatorIndex);
            auto * oscillatorSaw = sawoscillators.getUnchecked(oscillatorIndex);
            
            double gapToUse = detuneGap;
            
            if (oscillatorIndex % 2 == 0) gapToUse *= -1;
            
            oscillatorSine -> setFrequency((float) frequency + (oscillatorIndex * gapToUse), currentSampleRate);
            oscillatorSaw -> setFrequency((float) frequency + (oscillatorIndex * gapToUse), currentSampleRate);
            
            for (auto sample = 0; sample < bufferToFill.numSamples; ++sample) {
                auto sineSample = oscillatorSine -> getNextSample();
                auto sawSample = oscillatorSaw -> getNextSample();
                
                auto mix = (sineSample * balance) + (sawSample * (1.0 - balance));
                
                float levelSample = mix * level;
                
                levelSample *= adsr.getNextSample();
                
                leftBuffer[sample] += levelSample;
                rightBuffer[sample] += levelSample;
            }
        }
        
        
        visualiser.pushBuffer(bufferToFill);
    }
    
    
    
private:
    Label cpuUsageLabel;
    Label cpuUsageText;
    double currentSampleRate = 0.0;
    double frequency = 440.0;
    double balance = 0.5;
    double detuneGap = 0.5;
    double numOsc = 1.0;
    
    Visualiser visualiser;
    
    Label frequencyLabel;
    Slider frequencySlider;
    Label balanceLabel;
    Slider balanceSlider;
    Label detuneLabel;
    Slider detuneSlider;
    Label oscLabel;
    Slider oscSlider;
    
    TextButton triggerButton;
    
    ADSR adsr;
    
    WaveGenerator waveGen;
    WaveViz waveViz;
    
    AudioSampleBuffer* waveSpace;
    
    ButtonListener buttonListener;
    
    const unsigned int tableSize = 256; // [2]
    float level = 0.25f;
    
    AudioSampleBuffer sineTable; // [1]
    AudioSampleBuffer sawTable;
    OwnedArray < WavetableOscillator > sawoscillators;
    OwnedArray < WavetableOscillator > sineoscillators;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent);
    };
