/*
  ==============================================================================

    OscControls.h
    Created: 17 May 2019 9:39:32pm
    Author:  Matthew Martori

  ==============================================================================
*/


#pragma once
#include "WaveViz.h"


class OscControls: public Component {
public: OscControls() {
    
    addAndMakeVisible(waveViz);
    
    frequencyLabel.setJustificationType(Justification::left);
    frequencyLabel.setText("Frequency (Hz)", dontSendNotification);
    addAndMakeVisible(frequencyLabel);
    addAndMakeVisible(frequencySlider);
    frequencySlider.setRange(-3.0, 3.0);
    frequencySlider.setValue(0.0);
    frequencySlider.onValueChange = [this] {
        setParameters();
    };
    
    balanceLabel.setJustificationType(Justification::left);
    balanceLabel.setText("WT Pos", dontSendNotification);
    addAndMakeVisible(balanceLabel);
    addAndMakeVisible(balanceSlider);
    balanceSlider.setRange(0.0, 1.0);
    //        balanceSlider.setSkewFactorFromMidPoint (.05); // [4]
    balanceSlider.onValueChange = [this] {
        setParameters();
    };
    
    detuneLabel.setJustificationType(Justification::left);
    detuneLabel.setText("Detune (gap between osc in Hz)", dontSendNotification);
    addAndMakeVisible(detuneLabel);
    addAndMakeVisible(detuneSlider);
    detuneSlider.setRange(0.0, 1.0);
    //        balanceSlider.setSkewFactorFromMidPoint (.05); // [4]
    detuneSlider.onValueChange = [this] {
        setParameters();
    };
    
    oscLabel.setJustificationType(Justification::left);
    oscLabel.setText("Number of Oscillators", dontSendNotification);
    addAndMakeVisible(oscLabel);
    addAndMakeVisible(oscSlider);
    oscSlider.setRange(1.0, 8.0, 1.0);
    //        balanceSlider.setSkewFactorFromMidPoint (.05); // [4]
    oscSlider.onValueChange = [this] {
        setParameters();
    };
    
    
}
    
    void setParameters() {
        frequency = frequencySlider.getValue();
        balance = balanceSlider.getValue();
        detuneGap = detuneSlider.getValue();
        numOsc = oscSlider.getValue();
        
        waveViz.setWTPos(balance);
        
        if (onValueChange != nullptr) {
            onValueChange();
        }
        
    }
    
    
    struct oscParams {
        double frequency;
        double balance;
        double detuneGap;
        double numOsc;
    };
    
    oscParams getParameters() {
        oscParams params;
        
        params.frequency = frequency;
        params.balance = balance;
        params.detuneGap = detuneGap;
        params.numOsc = numOsc;
        
        return params;
    }
    
    std::function<void()> onValueChange;
    
    
    void resized() override {
        int const oscWidth = 400;
        
        waveViz.setBounds(0, 0, oscWidth - 20, 80);
        
        frequencyLabel.setBounds(0, 30 + 60, oscWidth - 20, 20);
        frequencySlider.setBounds(0, 50 + 60, oscWidth - 20, 20);
        
        balanceLabel.setBounds(0, 70 + 60, oscWidth - 20, 20);
        balanceSlider.setBounds(0, 90 + 60, oscWidth - 20, 20);
        
        detuneLabel.setBounds(0, 110 + 60, oscWidth - 20, 20);
        detuneSlider.setBounds(0, 130 + 60, oscWidth - 20, 20);
        
        oscLabel.setBounds(0, 150 + 60, oscWidth - 20, 20);
        oscSlider.setBounds(0, 170 + 60, oscWidth - 20, 20);
    }
    
    void setWaveTables (AudioSampleBuffer tableA, AudioSampleBuffer tableB) {
        waveViz.setWaveTables(tableA, tableB);
        waveViz.setWTPos(0.5);
    }
    
    Boolean changed = false;
    
private:
    
    Label frequencyLabel;
    Slider frequencySlider;
    Label balanceLabel;
    Slider balanceSlider;
    Label detuneLabel;
    Slider detuneSlider;
    Label oscLabel;
    Slider oscSlider;
    
    double frequency = 440.0;
    double balance = 0.0;
    double detuneGap = 0.5;
    double numOsc = 1.0;
    
    WaveViz waveViz;
    
};
