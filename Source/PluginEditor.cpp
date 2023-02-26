/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FuzzPedalAudioProcessorEditor::FuzzPedalAudioProcessorEditor (FuzzPedalAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (300, 400);
    
    // Gain slider
    gainSlider.setSliderStyle(juce::Slider::Slider::LinearBarVertical);
    gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 100, 25);
    gainSlider.addListener(this);

    addAndMakeVisible(gainSlider);
    gainLabel.setText("FUZZ", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, false);
    
    gainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "THRESHOLD", gainSlider);
    
    // Mix slider
    mixSlider.setSliderStyle(juce::Slider::Slider::LinearBarVertical);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 100, 25);
    mixSlider.addListener(this);

    addAndMakeVisible(mixSlider);
    mixLabel.setText("MIX", juce::dontSendNotification);
    mixLabel.attachToComponent(&mixSlider, false);
    
    mixSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "MIX_RATIO", mixSlider);
    
    // Cutoff
    cutoffSlider.setSliderStyle(juce::Slider::Slider::RotaryHorizontalVerticalDrag);
    cutoffSlider.addListener(this);
    cutoffSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 100, 25);
    addAndMakeVisible(cutoffSlider);
    
    cutoffSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "CUTOFF", cutoffSlider);
    
    // Boost
    boostToggle.setButtonText("BOOST");
    addAndMakeVisible(boostToggle);
    
    boostToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.tree, "BOOST", boostToggle);
    
    // Styling
    mixLookAndFeel.setColour(juce::Slider::trackColourId, juce::Colour::fromRGB(72,105,102));
    mixLookAndFeel.setColour(juce::Slider::backgroundColourId, juce::Colour::fromRGB(59,57,53));
    gainLookAndFeel.setColour(juce::Slider::trackColourId, juce::Colour::fromRGB(59,57,53));
    
    cutoffLookAndFeel.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(255,207,189));
    
    mixSlider.setLookAndFeel(&mixLookAndFeel);
    gainSlider.setLookAndFeel(&gainLookAndFeel);
    cutoffSlider.setLookAndFeel(&cutoffLookAndFeel);
    

}

FuzzPedalAudioProcessorEditor::~FuzzPedalAudioProcessorEditor()
{
}

//==============================================================================
void FuzzPedalAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::orangered);
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void FuzzPedalAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    cutoffSlider.setBounds(165, 0, 100, 100);
    gainSlider.setBounds(40, 50, 100, 325);
    mixSlider.setBounds(165, 150, 100, 225);
    boostToggle.setBounds(165, 100, 100, 20);
    
}

void FuzzPedalAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &gainSlider)
    {
        // log scale for vol changes
        //audioProcessor.rawVolume = pow(10, gainSlider.getValue() / 20);
        audioProcessor.rawVolume = gainSlider.getValue();
    } else if (slider == &mixSlider)
    {
        audioProcessor.mixRatio = mixSlider.getValue();
    }
}
