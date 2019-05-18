/*
  ==============================================================================

    ButtonListener.h
    Created: 17 May 2019 1:53:23pm
    Author:  Matthew Martori

  ==============================================================================
*/

#pragma once

class ButtonListener : public MouseListener
{
public:
    
    std::function<void(const MouseEvent&)> onMouseUp;
    std::function<void(const MouseEvent&)> onMouseDown;
    
    void mouseUp (const MouseEvent& e) override
    {
        if (onMouseUp) onMouseUp(e);
    }
    
    void mouseDown (const MouseEvent& e) override
    {
        if (onMouseDown) onMouseDown(e);
    }
    
};
