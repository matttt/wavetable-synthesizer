/*
  ==============================================================================

    WaveViz.h
    Created: 17 May 2019 1:51:28pm
    Author:  Matthew Martori

  ==============================================================================
*/

#pragma once
#include <string>
using namespace std;

class WaveViz: public Component, public Timer {
public: WaveViz() {
    startTimer(50);
}
    
    void setWaveTables (AudioSampleBuffer tableA_, AudioSampleBuffer tableB_) {
        tableA = tableA_;
        tableB = tableB_;
        wTSet = true;
    }
    
    void timerCallback() override {
        repaint();
    }
    
    void setWTPos(int wtPos_) {
        wtPos = wtPos_;
    }
    
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::black);
        g.setColour (Colours::darkred);
        
        int w = getWidth();
        int h = getHeight();
        
        if (wTSet) {
            int tableSize = tableA.getNumSamples();
            const auto * tableAArr = tableA.getReadPointer(0); // [8]
            const auto * tableBArr = tableB.getReadPointer(0); // [8]

            
            
            float pixelsPerSample = float(w) / float(tableSize);
        
            for(int i = 0; i < tableSize; ++i) {
                
                auto sampleA = tableAArr[i];
                auto sampleB = tableBArr[i];
                
                float balance = float(wtPos) / 1024.0;
                
                float value = (sampleA * balance) + (sampleB * (1.0 - balance));
                int x = int(i*pixelsPerSample);
                int y = int((h/2) + int((value) * h*0.5));
                
                if (x > 0 && x < w && y > 0 && y < h) {
                    g.drawRect (x-1, y-1, 3, 3);
                } else {
                    continue;
                }
                
            }
            
        }
        
    }


    
private:
    AudioSampleBuffer tableA;
    AudioSampleBuffer tableB;
    
    int wtPos = 0;
    Boolean wTSet = false;
    
};

