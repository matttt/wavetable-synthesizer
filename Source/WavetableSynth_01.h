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
#include "ButtonListener.h"
#include "OscControls.h"
#include "FilterControls.cpp"
#pragma once
#include "Synthesizer.cpp"



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
public Timer
{
    
    
public: MainContentComponent() :
    synthAudioSource (keyboardState, oscControls, adsrControls)
    {
    

    addAndMakeVisible (midiInputListLabel);
    midiInputListLabel.setText ("MIDI Input:", dontSendNotification);
    midiInputListLabel.attachToComponent (&midiInputList, true);
    addAndMakeVisible (midiInputList);
    midiInputList.setTextWhenNoChoicesAvailable ("No MIDI Inputs Enabled");
    auto midiInputs = MidiInput::getDevices();
    midiInputList.addItemList (midiInputs, 1);
    midiInputList.onChange = [this] { setMidiInput (midiInputList.getSelectedItemIndex()); };
    for (auto midiInput : midiInputs)
    {
        if (deviceManager.isMidiInputEnabled (midiInput))
        {
            setMidiInput (midiInputs.indexOf (midiInput));
            break;
        }
    }
    if (midiInputList.getSelectedId() == 0)
        setMidiInput (0);
        
//    setMidiInput ();

    
    cpuUsageLabel.setText("CPU Usage", dontSendNotification);
    cpuUsageText.setJustificationType(Justification::right);
    addAndMakeVisible(cpuUsageLabel);
    addAndMakeVisible(cpuUsageText);
    
    
    addAndMakeVisible(oscControls);
        
    addAndMakeVisible(adsrControls);
    addAndMakeVisible(filterControls);
    
    addAndMakeVisible(visualiser);
    
    
    visualiser.clear();

    aTable = waveGen.createSineWavetable(aTable, 512);
    bTable = waveGen.createSawWavetable(bTable, 512);
        
        
    oscControls.setWaveTables(aTable, bTable);
    oscControls.setParameters();
        
        
        
        
    setSize(800, 500);
    setAudioChannels(0, 2); // no inputs, two outputs
    startTimer(50);
}
    
    ~MainContentComponent() {
        shutdownAudio();
    }
    
    void resized() override {
        int const oscWidth = 400;
        
        cpuUsageLabel.setBounds(10, 10, oscWidth - 20, 20);
        cpuUsageText.setBounds(10, 10, oscWidth - 20, 20);
        
        oscControls.setBounds(10, 40, oscWidth, 250);
        
        adsrControls.setBounds(oscWidth + 20, 40, oscWidth, 250);
        
        filterControls.setBounds(oscWidth + 20, getHeight()- 80, oscWidth, 250);
        
        visualiser.setBounds(10, 200 + 100, oscWidth - 20, 80);
        
        midiInputList.setBounds(70, getHeight()-30, 40, 20);
    }

    
    void prepareToPlay(int sPB, double sampleRate) override {
        currentSampleRate = sampleRate;
        synthAudioSource.prepareToPlay(sPB, sampleRate);
    }
    
    void releaseResources() override {}

    
    void getNextAudioBlock(const AudioSourceChannelInfo & bufferToFill) override {
        visualiser.setSPB(bufferToFill.numSamples);
        FilterControls::Parameters filterParams = filterControls.getParameters();
        
        double freq = filterParams.freq + .01;
        double res = filterParams.res + .01;
        
        filter.setCoefficients(IIRCoefficients::makeLowPass(currentSampleRate, freq, res));
    
        // apply osc to output
        synthAudioSource.getNextAudioBlock (bufferToFill);
        
        // apply filter to output
//        for (int i = 0; i <= 1; i++) {
//            auto buffer = bufferToFill.buffer->getWritePointer(i);
//            filter.processSamples(buffer, bufferToFill.numSamples);
//        }
        
        
        
        visualiser.pushBuffer(bufferToFill);
    }
    
    
    
private:
    
    void timerCallback() override {
        auto cpu = deviceManager.getCpuUsage() * 100;
        cpuUsageText.setText(String(cpu, 6) + " %", dontSendNotification);
        
    }
    
    void setMidiInput (int index)
    {
        auto list = MidiInput::getDevices();
        
        deviceManager.removeMidiInputCallback (list[lastInputIndex], synthAudioSource.getMidiCollector());
        
        auto newInput = list[index];
        
        if (! deviceManager.isMidiInputEnabled (newInput))
            deviceManager.setMidiInputEnabled (newInput, true);
        
        deviceManager.addMidiInputCallback (newInput, synthAudioSource.getMidiCollector());
        midiInputList.setSelectedId (index + 1, dontSendNotification);
        
        lastInputIndex = index;
    }
    
    Label cpuUsageLabel;
    Label cpuUsageText;
    OscControls oscControls;
    ADSRControls adsrControls;
    FilterControls filterControls;
    Visualiser visualiser;
    
    SynthAudioSource synthAudioSource;
    MidiKeyboardState keyboardState;
    
    IIRFilter filter;
    
    ComboBox midiInputList;
    Label midiInputListLabel;
    int lastInputIndex = 0;
    
    double currentSampleRate = 0.0;
    double level = 0.25f;
    double frequency = 440.0;
    
    
    WaveGenerator waveGen;

    
    ButtonListener buttonListener;
    
    const unsigned int tableSize = 512; // [2]
    
    AudioSampleBuffer aTable;
    AudioSampleBuffer bTable;

  
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent);
    };


