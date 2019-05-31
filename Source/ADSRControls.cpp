/*
  ==============================================================================

    ADSRControls.cpp
    Created: 18 May 2019 11:14:57pm
    Author:  Matthew Martori

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"




class ADSRControls: public Component {
public: ADSRControls() {
    
    
    attackLabel.setJustificationType(Justification::left);
    attackLabel.setText("Attack", dontSendNotification);
    addAndMakeVisible(attackLabel);
    addAndMakeVisible(attackSlider);
    attackSlider.setRange(0, 3.0);
    attackSlider.setValue(0.3);
    attackSlider.setSkewFactorFromMidPoint(0.5);
    attackSlider.onValueChange = [this] {
        setParameters();
    };
    
    decayLabel.setJustificationType(Justification::left);
    decayLabel.setText("Decay", dontSendNotification);
    addAndMakeVisible(decayLabel);
    addAndMakeVisible(decaySlider);
    decaySlider.setRange(0.0, 3.0);
    decaySlider.setSkewFactorFromMidPoint(0.5);
    //        decaySlider.setSkewFactorFromMidPoint (.05); // [4]
    decaySlider.onValueChange = [this] {
        setParameters();
    };
    
    sustainLabel.setJustificationType(Justification::left);
    sustainLabel.setText("Sustain", dontSendNotification);
    addAndMakeVisible(sustainLabel);
    addAndMakeVisible(sustainSlider);
    sustainSlider.setRange(0.0, 1.0);
    sustainSlider.setValue(1.0);
    sustainSlider.onValueChange = [this] {
        setParameters();
    };
    
    releaseLabel.setJustificationType(Justification::left);
    releaseLabel.setText("Release", dontSendNotification);
    addAndMakeVisible(releaseLabel);
    addAndMakeVisible(releaseSlider);
    releaseSlider.setRange(0.0, 8.0);
    releaseSlider.setValue(0.5);
    releaseSlider.setSkewFactorFromMidPoint (0.5); // [4]
    releaseSlider.onValueChange = [this] {
        setParameters();
    };
    
    
}
    
    void setParameters() {
        attack = attackSlider.getValue();
        decay = decaySlider.getValue();
        sustain = sustainSlider.getValue();
        release = releaseSlider.getValue();
        
        
        if (onValueChange != nullptr) {
            onValueChange();
        }
        
    }
    
    
    
    ADSR::Parameters getParameters() {
        ADSR::Parameters params;
        
        params.attack = attack;
        params.decay = decay;
        params.sustain = sustain;
        params.release = release;
        
        return params;
    }
    
    std::function<void()> onValueChange;
    
    
    void resized() override {
        int const oscWidth = 400;
                
        attackLabel.setBounds(0, 30 + 60, oscWidth - 20, 20);
        attackSlider.setBounds(0, 50 + 60, oscWidth - 20, 20);
        
        decayLabel.setBounds(0, 70 + 60, oscWidth - 20, 20);
        decaySlider.setBounds(0, 90 + 60, oscWidth - 20, 20);
        
        sustainLabel.setBounds(0, 110 + 60, oscWidth - 20, 20);
        sustainSlider.setBounds(0, 130 + 60, oscWidth - 20, 20);
        
        releaseLabel.setBounds(0, 150 + 60, oscWidth - 20, 20);
        releaseSlider.setBounds(0, 170 + 60, oscWidth - 20, 20);
    }
    
    
private:
    
    Label attackLabel;
    Slider attackSlider;
    Label decayLabel;
    Slider decaySlider;
    Label sustainLabel;
    Slider sustainSlider;
    Label releaseLabel;
    Slider releaseSlider;
    
    double attack = 440.0;
    double decay = 0.0;
    double sustain = 0.5;
    double release = 1.0;
    
};
