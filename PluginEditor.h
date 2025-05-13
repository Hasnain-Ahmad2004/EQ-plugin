/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


struct CustomRotarySlider :juce::Slider {

    CustomRotarySlider():
        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
            juce::Slider::TextEntryBoxPosition::NoTextBox)
    {}
};

//==============================================================================
/**
*/
struct ResponseCurveComponent :public juce::Component, public juce::AudioProcessorParameter::Listener,
    public juce::Timer
{
    ResponseCurveComponent(NewProjectAudioProcessor&);
    ~ResponseCurveComponent();
    void parameterValueChanged(int parameterIndex, float newValue)override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting)override;

    void timerCallback()override;
    void paint(juce::Graphics& g) override;
private:
    NewProjectAudioProcessor& audioProcessor;
    juce::Atomic<bool>parametersChanged{ false };
    MonoChain monoChain;

};

class NewProjectAudioProcessorEditor  : public juce::AudioProcessorEditor
{ 
public:
    NewProjectAudioProcessorEditor (NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NewProjectAudioProcessor& audioProcessor;
    

    CustomRotarySlider peakFreqSlider,peakGainSlider,peakQualitySlider,lowCutFreqSlider,highCutFreqSlider,lowCutSlopeSlider,highCutSlopeSlider;


    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;
    
    Attachment peakFreqSliderAttachment,
        peakGainSliderAttachment,
        peakQualitySliderAttachment,
        lowCutFreqSliderAttachment,
        highCutFreqSliderAttachment,
        lowCutSlopeSliderAttachment,
        highCutSlopeSliderAttachment;

    ResponseCurveComponent responseCurveComponent;


    std::vector<juce::Component*> getComps();
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};
