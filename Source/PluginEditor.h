/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class FuzzPedalAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                   public juce::Slider::Listener
{
public:
    FuzzPedalAudioProcessorEditor (FuzzPedalAudioProcessor&);
    ~FuzzPedalAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void sliderValueChanged(juce::Slider *slider) override;
    //juce::ScopedPointer <juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttach;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    
    juce::Slider gainSlider;
    juce::Label gainLabel;
    juce::Slider mixSlider;
    juce::Label mixLabel;
    juce::Slider cutoffSlider;
    juce::Label cutoffLabel;
    juce::ToggleButton boostToggle;
    
    juce::LookAndFeel_V4 gainLookAndFeel;
    juce::LookAndFeel_V4 mixLookAndFeel;
    juce::LookAndFeel_V4 cutoffLookAndFeel; 
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> boostToggleAttachment;
    
    FuzzPedalAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FuzzPedalAudioProcessorEditor)
};
