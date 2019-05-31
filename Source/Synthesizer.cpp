/*
  ==============================================================================

    Synthesizer.cpp
    Created: 18 May 2019 3:21:55am
    Author:  Matthew Martori

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Voice.cpp"



class SynthAudioSource   : public AudioSource
{
public:
    SynthAudioSource (MidiKeyboardState& keyState, OscControls& oc, ADSRControls& adsrC)
    : keyboardState (keyState), oscControls (oc), adsrControls (adsrC)
    {
        for (auto i = 0; i < 8; ++i){
            Voice * newVoice = new Voice(oc, adsrC, i);// [1]
            synth.addVoice(newVoice);
        
        }
        synth.addSound (new WaveTableSound());
    }
    
    void setUsingSineWaveSound()
    {
        synth.clearSounds();
    }
    
    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        synth.setCurrentPlaybackSampleRate (sampleRate);
        midiCollector.reset (sampleRate); // [10]
    }
    
    void releaseResources() override {}
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        bufferToFill.clearActiveBufferRegion();
        
        MidiBuffer incomingMidi;
        midiCollector.removeNextBlockOfMessages (incomingMidi, bufferToFill.numSamples); // [11]
        
        keyboardState.processNextMidiBuffer (incomingMidi, bufferToFill.startSample,
                                             bufferToFill.numSamples, true);
        
        synth.renderNextBlock (*bufferToFill.buffer, incomingMidi,
                               bufferToFill.startSample, bufferToFill.numSamples);
    }
    
    MidiMessageCollector* getMidiCollector()
    {
        return &midiCollector;
    }
private:
    MidiKeyboardState& keyboardState;
    OscControls& oscControls;
    ADSRControls& adsrControls;
    Synthesiser synth;
    
    MidiMessageCollector midiCollector;
};

