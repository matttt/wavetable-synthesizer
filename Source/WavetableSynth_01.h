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
    
    AudioDeviceManager deviceManager;           // [1]
    ComboBox midiInputList;                     // [2]
    Label midiInputListLabel;
    int lastInputIndex = 0;                     // [3]
    bool isAddingFromMidiInput = false;         // [4]
    MidiKeyboardState keyboardState;            // [5]
    double startTime;
    TextEditor midiMessagesBox;
    
    
    int latestNote = 0;
    
public: MainContentComponent() :
    startTime (Time::getMillisecondCounterHiRes() * 0.001),
    synthAudioSource (keyboardState, oscControls)
    {
    

        addAndMakeVisible (midiInputList);
        midiInputList.setTextWhenNoChoicesAvailable ("No MIDI Inputs Enabled");
        auto midiInputs = MidiInput::getDevices();
        midiInputList.addItemList (midiInputs, 1);
        midiInputList.onChange = [this] { setMidiInput (midiInputList.getSelectedItemIndex()); };
        // find the first enabled device and use that by default
        for (auto midiInput : midiInputs)
        {
            if (deviceManager.isMidiInputEnabled (midiInput))
            {
                setMidiInput (midiInputs.indexOf (midiInput));
                break;
            }
        }
        // if no enabled devices were found just use the first one in the list
        if (midiInputList.getSelectedId() == 0)
            setMidiInput (0);
        
//        keyboardState.addListener (this);
    
        
        
        
    midiInputList.onChange = [this] { setMidiInput (midiInputList.getSelectedItemIndex()); };
    

    
    cpuUsageLabel.setText("CPU Usage", dontSendNotification);
    cpuUsageText.setJustificationType(Justification::right);
    addAndMakeVisible(cpuUsageLabel);
    addAndMakeVisible(cpuUsageText);
    
    
    addAndMakeVisible(oscControls);
    
//    oscControls.onValueChange = [this] {
//        oscParams params = oscControls.getParameters();
//
//        frequency = params.frequency;
//        balance = params.balance;
//        detuneGap = params.detuneGap;
//        numOsc = params.numOsc;
//    }
//
    addAndMakeVisible(triggerButton);
    triggerButton.setButtonText("Trigger");
    triggerButton.addMouseListener(&buttonListener, false);
    
    
    buttonListener.onMouseUp = [this](const MouseEvent& e) {
        adsr.noteOff();
    };
    
    buttonListener.onMouseDown = [this](const MouseEvent& e) {
        adsr.noteOn();
    };
    
    addAndMakeVisible(visualiser);
    
    
    visualiser.clear();

    AudioSampleBuffer sineTable = waveGen.createSineWavetable(1 << 8);
    AudioSampleBuffer sawTable = waveGen.createSawWavetable(1 << 8);
        
        
    oscControls.setWaveTables(sineTable, sawTable);
        
        
    
    adsr.setParameters({
        .attack =  .03f,
        .decay = 0.3f,
        .sustain = 1.0f,
        .release = 0.0f
    });
    
    setSize(400, 420);
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
    
//
    
//    void handleIncomingMidiMessage (MidiInput* source, const MidiMessage& message) override
//    {
//        const ScopedValueSetter<bool> scopedInputFlag (isAddingFromMidiInput, true);
////        keyboardState.processNextMidiEvent (message);
//
////        postMessageToList (message, source->getName());
//    }
//
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
    
    
    
    
//    void postMessageToList (const MidiMessage& message, const String& source)
//    {
////        (new IncomingMessageCallback (this, message, source))->post();
//
//        MessageManager* mm = MessageManager::getInstance();
//
//        mm.MessageBase->post((this, message, source));
//
//    }
    
//    void addMessageToList (const MidiMessage& message, const String& source)
//    {
//        auto time = message.getTimeStamp() - startTime;
//        auto hours   = ((int) (time / 3600.0)) % 24;
//        auto minutes = ((int) (time / 60.0)) % 60;
//        auto seconds = ((int) time) % 60;
//        auto millis  = ((int) (time * 1000.0)) % 1000;
//        auto timecode = String::formatted ("%02d:%02d:%02d.%03d",
//                                           hours,
//                                           minutes,
//                                           seconds,
//                                           millis);
//        auto description = getMidiMessageDescription (message);
//        String midiMessageString (timecode + "  -  " + description + " (" + source + ")"); // [7]
//        logMessage (midiMessageString);
//    }
    
    //...
    
    //...
    
    
    
    void resized() override {
        int const oscWidth = 400;
        
        cpuUsageLabel.setBounds(10, 10, oscWidth - 20, 20);
        cpuUsageText.setBounds(10, 10, oscWidth - 20, 20);
        
        oscControls.setBounds(10, 40, oscWidth, 250);
        
        triggerButton.setBounds(oscWidth - 80, 170 + 100, 70, 20);
        
        visualiser.setBounds(10, 200 + 100, oscWidth - 20, 80);
        
        midiInputList.setBounds(getWidth() - 50, getHeight()-20, 40, 20);
    }
    
    void timerCallback() override {
        auto cpu = deviceManager.getCpuUsage() * 100;
        cpuUsageText.setText(String(cpu, 6) + " %", dontSendNotification);
        
    }
    
    
    void prepareToPlay(int sPB, double sampleRate) override {
        
//        level = 0.25f / numberOfOscillators;
        
        synthAudioSource.prepareToPlay(sPB, sampleRate);
    }
    
    void releaseResources() override {}

    
    void getNextAudioBlock(const AudioSourceChannelInfo & bufferToFill) override {
        visualiser.setSPB(bufferToFill.numSamples);
        
        
        synthAudioSource.getNextAudioBlock (bufferToFill);
        
        
        visualiser.pushBuffer(bufferToFill);
    }
    
    
    
private:
    Label cpuUsageLabel;
    Label cpuUsageText;
    OscControls oscControls;
    Visualiser visualiser;
    TextButton triggerButton;
    
    double currentSampleRate = 0.0;
    double level = 0.25f;
    double frequency = 440.0;
    
    ADSR adsr;
    
    WaveGenerator waveGen;

    
    ButtonListener buttonListener;
    
    const unsigned int tableSize = 256; // [2]
    
    AudioSampleBuffer sineTable; // [1]
    AudioSampleBuffer sawTable;

    SynthAudioSource synthAudioSource;
  
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent);
    };


