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
    
    gainSlider.setSliderStyle(juce::Slider::Slider::LinearVertical);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 25);
    gainSlider.addListener(this);
    //gainSlider.setRange(0.0f, 0.065f);
    //gainSlider.setValue(0.01);
    addAndMakeVisible(gainSlider);
    gainLabel.setText("Threshold", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, false);
    
    gainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "THRESHOLD", gainSlider);
    
    mixSlider.setSliderStyle(juce::Slider::Slider::LinearVertical);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 25);
    mixSlider.addListener(this);
    //mixSlider.setRange(0.0f, 1.0f);
    //mixSlider.setValue(0.5f);
    addAndMakeVisible(mixSlider);
    mixLabel.setText("Wet/Dry Ratio", juce::dontSendNotification);
    mixLabel.attachToComponent(&mixSlider, false);
    
    mixSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "MIX_RATIO", mixSlider);
}

FuzzPedalAudioProcessorEditor::~FuzzPedalAudioProcessorEditor()
{
}

//==============================================================================
void FuzzPedalAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void FuzzPedalAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    gainSlider.setBounds(40, 50, 100, 325);
    mixSlider.setBounds(165, 50, 100, 325);
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
