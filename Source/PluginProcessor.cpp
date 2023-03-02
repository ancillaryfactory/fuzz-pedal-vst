/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FuzzPedalAudioProcessor::FuzzPedalAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), tree(*this, nullptr, "Parameters", createParameters()),
lowPassFilter(juce::dsp::FilterDesign<float>::designFIRLowpassWindowMethod(20000.0f, 44100, 21, juce::dsp::WindowingFunction<float>::hamming))

#endif
{
    
}

FuzzPedalAudioProcessor::~FuzzPedalAudioProcessor()
{
}

//==============================================================================
const juce::String FuzzPedalAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FuzzPedalAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FuzzPedalAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FuzzPedalAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FuzzPedalAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FuzzPedalAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FuzzPedalAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FuzzPedalAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FuzzPedalAudioProcessor::getProgramName (int index)
{
    return {};
}

void FuzzPedalAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FuzzPedalAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    lastSampleRate = sampleRate;
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    lowPassFilter.reset();
    updateLowPassFilter();
    lowPassFilter.prepare(spec);
    
}

void FuzzPedalAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FuzzPedalAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

/*
double applyFuzz(double input, double threshold)
{
    // apply fuzz effect
    double output = input * input;
    output *= 2.0;
    if (output > 1.0)
    {
        output = 1.0;
    }
    return output;
}
*/

void FuzzPedalAudioProcessor::updateLowPassFilter()
{
    float freq = *tree.getRawParameterValue("CUTOFF");
    *lowPassFilter.state = *juce::dsp::FilterDesign<float>::designFIRLowpassWindowMethod(freq, 44100, 21, juce::dsp::WindowingFunction<float>::hamming);
}

void FuzzPedalAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    juce::dsp::AudioBlock<float> block(buffer);
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {

            auto input = channelData[sample];
            auto cleanOut = channelData[sample];

            auto threshold = tree.getRawParameterValue("THRESHOLD");
            auto mixRatio = tree.getRawParameterValue("MIX_RATIO");
            auto boost = tree.getRawParameterValue("BOOST");
            
            float rand_num = (std::rand() / RAND_MAX);

            // rand/RAND_MAX will be between 0 and 1
            float jitter = rand_num / 100;
            if (input > threshold->load() + jitter)
            {
                input = input;
            }
            else
            {
                input = (boost->load()) ? 1 : 0;
            }


            channelData[sample] = ((1 - mixRatio->load()) * cleanOut) + (mixRatio->load() * input);

        }
   }
    
    updateLowPassFilter();
    lowPassFilter.process(juce::dsp::ProcessContextReplacing<float> (block));
}

//==============================================================================
bool FuzzPedalAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FuzzPedalAudioProcessor::createEditor()
{
    return new FuzzPedalAudioProcessorEditor (*this);
}

//==============================================================================
void FuzzPedalAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    // https://docs.juce.com/master/tutorial_audio_processor_value_tree_state.html
    
    auto state = tree.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void FuzzPedalAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(tree.state.getType()))
        {
            tree.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FuzzPedalAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout FuzzPedalAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    juce::NormalisableRange<float> cutoffRange(20.0f, 6000.0f);
    juce::NormalisableRange<float> thresholdRange(0.0f, 0.1f);
    juce::NormalisableRange<float> mixRange(0.0f, 1.0f);
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("THRESHOLD", "Threshold", thresholdRange, 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("MIX_RATIO", "Mix Ratio", mixRange, 0.5f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("CUTOFF", "cutoff", cutoffRange, 10000.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("BOOST", "Boost", false));
    
    return {params.begin(), params.end()};
}
