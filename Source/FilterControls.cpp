/*
  ==============================================================================

    FilterControls.cpp
    Created: 20 May 2019 1:32:05am
    Author:  Matthew Martori

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"




class FilterControls: public Component {
public: FilterControls() {
    
    
    attackLabel.setJustificationType(Justification::left);
    attackLabel.setText("Frequency", dontSendNotification);
    addAndMakeVisible(attackLabel);
    addAndMakeVisible(freqSlider);
    freqSlider.setRange(0, 15000.0);
    freqSlider.setValue(0.3);
    freqSlider.setSkewFactorFromMidPoint(6000.0);
    freqSlider.onValueChange = [this] {
        setParameters();
    };
    
    resLabel.setJustificationType(Justification::left);
    resLabel.setText("Resonance", dontSendNotification);
    addAndMakeVisible(resLabel);
    addAndMakeVisible(resSlider);
    resSlider.setRange(0.0, 1.0);
    resSlider.setSkewFactorFromMidPoint(0.5);
    //        resSlider.setSkewFactorFromMidPoint (.05); // [4]
    resSlider.onValueChange = [this] {
        setParameters();
    };
    
    
    
    
}
    
    void setParameters() {
        freq = freqSlider.getValue();
        res = resSlider.getValue();

        
        
        if (onValueChange != nullptr) {
            onValueChange();
        }
        
    }
    
    struct Parameters {
        double freq;
        double res;
    };
    
    Parameters getParameters() {
        Parameters params;
        
        params.freq = freq;
        params.res = res;
        
        return params;
    }
    
    std::function<void()> onValueChange;
    
    
    void resized() override {
        int const oscWidth = 400;
        
        attackLabel.setBounds(0, 30 - 30, oscWidth - 20, 20);
        freqSlider.setBounds(0, 50 - 30, oscWidth - 20, 20);
        
        resLabel.setBounds(0, 70 - 30, oscWidth - 20, 20);
        resSlider.setBounds(0, 90 - 30, oscWidth - 20, 20);

    }
    
    
private:
    
    Label attackLabel;
    Slider freqSlider;
    Label resLabel;
    Slider resSlider;
    
    double freq = 440.0;
    double res = 0.0;
//    double sustain = 0.5;
//    double release = 1.0;
    
};
