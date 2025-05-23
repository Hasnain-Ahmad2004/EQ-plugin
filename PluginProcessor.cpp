/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
NewProjectAudioProcessor::NewProjectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

NewProjectAudioProcessor::~NewProjectAudioProcessor()
{
}

//==============================================================================
const juce::String NewProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NewProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NewProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NewProjectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NewProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NewProjectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NewProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void NewProjectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void NewProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;
    leftChain.prepare(spec);
    rightChain.prepare(spec);
    /*auto chainSettings = getChainSettings(apvts);

    updatePeakFilter(chainSettings);

    
    auto lowCutCoefficients= juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq, sampleRate,2*(chainSettings.lowCutSlope+1));

    auto& leftLowCut = leftChain.get<ChainPositions::Lowcut>();
    
    updateCutFilter(leftLowCut, lowCutCoefficients, chainSettings.lowCutSlope);

   
    auto& rightLowCut = rightChain.get<ChainPositions::Lowcut>();
    updateCutFilter(rightLowCut, lowCutCoefficients, chainSettings.lowCutSlope);
    
    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.highCutFreq, sampleRate
        , 2 * (chainSettings.highCutSlope + 1));

    auto& leftHighCut = leftChain.get<ChainPositions::HighCut>();

    auto& rightHighCut = rightChain.get<ChainPositions::HighCut>();

    
    updateCutFilter(leftHighCut, highCutCoefficients, chainSettings.highCutSlope);

    updateCutFilter(leftHighCut, highCutCoefficients, chainSettings.highCutSlope);*/

    updateFilters();


}


void NewProjectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NewProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void NewProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
   
    
    /*auto chainSettings = getChainSettings(apvts);

    updatePeakFilter(chainSettings);

    

    auto cutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq, getSampleRate(), 2 * (chainSettings.lowCutSlope + 1));

    auto& leftLowCut = leftChain.get<ChainPositions::Lowcut>();
    updateCutFilter(leftLowCut, cutCoefficients, chainSettings.lowCutSlope);
    

    auto& rightLowCut = rightChain.get<ChainPositions::Lowcut>();
    updateCutFilter(rightLowCut, cutCoefficients, chainSettings.lowCutSlope);

    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.highCutFreq, getSampleRate()
        , 2 * (chainSettings.highCutSlope + 1));

    auto& leftHighCut = leftChain.get<ChainPositions::HighCut>();

    auto& rightHighCut = rightChain.get<ChainPositions::HighCut>();


    updateCutFilter(leftHighCut, highCutCoefficients, chainSettings.highCutSlope);

    updateCutFilter(leftHighCut, highCutCoefficients, chainSettings.highCutSlope);*/


    updateFilters();


    juce::dsp::AudioBlock<float> block(buffer);
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);
    leftChain.process(leftContext);
    rightChain.process(rightContext);

}

//==============================================================================
bool NewProjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* NewProjectAudioProcessor::createEditor()
{
    return new NewProjectAudioProcessorEditor (*this);
    /*return new juce::GenericAudioProcessorEditor(*this);*/
}

//==============================================================================
void NewProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
         
}

void NewProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid()) {
        apvts.replaceState(tree);
        updateFilters();
    }
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts) {
    ChainSettings settings;

    settings.lowCutFreq= apvts.getRawParameterValue("LowCut Freq")->load();
    settings.highCutFreq = apvts.getRawParameterValue("HighCut Freq")->load();
    settings.peakFreq = apvts.getRawParameterValue("Peak Freq")->load();
    settings.peakGaininDecibels = apvts.getRawParameterValue("Peak Gain")->load();
    settings.peakQuality = apvts.getRawParameterValue("Peak Quality")->load();
    settings.lowCutSlope = static_cast<Slope>(apvts.getRawParameterValue("LowCut Slope")->load());
    settings.highCutSlope = static_cast<Slope>(apvts.getRawParameterValue("HighCut Slope")->load());


    return settings;
}

Coefficients makePeakFilter(const ChainSettings& chainSettings, double sampleRate)
{
    return juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 
        chainSettings.peakFreq, chainSettings.peakQuality, 
        juce::Decibels::decibelsToGain(chainSettings.peakGaininDecibels));

}

void NewProjectAudioProcessor::updatePeakFilter(const ChainSettings& chainSettings) {
    /*auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), chainSettings.peakFreq, chainSettings.peakQuality,
        juce::Decibels::decibelsToGain(chainSettings.peakGaininDecibels));*/

    auto peakCoefficients = makePeakFilter(chainSettings, getSampleRate());

    updateCoefficients(leftChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);

    updateCoefficients(rightChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
}
 void updateCoefficients(Coefficients& old, const Coefficients& replacements) {
     *old = *replacements;

}
 void NewProjectAudioProcessor::updateLowCutFilters(const ChainSettings& chainSettings) {
     auto cutCoefficients = makeLowCutFilter(chainSettings, getSampleRate());


     auto& leftLowCut = leftChain.get<ChainPositions::Lowcut>();
    

     auto& rightLowCut = rightChain.get<ChainPositions::Lowcut>();
     updateCutFilter(rightLowCut, cutCoefficients, chainSettings.lowCutSlope);
     updateCutFilter(leftLowCut, cutCoefficients, chainSettings.lowCutSlope);
 }

 void NewProjectAudioProcessor::updateHighCutFilters(const ChainSettings& chainSettings)
 {
     auto highCutCoefficients = makeHighCutFilter(chainSettings, getSampleRate());


     auto& leftHighCut = leftChain.get<ChainPositions::HighCut>();

     auto& rightHighCut = rightChain.get<ChainPositions::HighCut>();


     updateCutFilter(leftHighCut, highCutCoefficients, chainSettings.highCutSlope);

     updateCutFilter(rightHighCut, highCutCoefficients, chainSettings.highCutSlope);




 }

 void NewProjectAudioProcessor::updateFilters()
 {
     auto chainSettings = getChainSettings(apvts);

     updateLowCutFilters(chainSettings);
     updatePeakFilter(chainSettings);
     updateHighCutFilters(chainSettings);

 }

juce::AudioProcessorValueTreeState::ParameterLayout 

NewProjectAudioProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>("LowCut Freq", "LowCut Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 20.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("HighCut Freq", "HighCut Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 20000.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Freq", "Peak Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 750.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Gain", "Peak Gain",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Quality", "Peak Quality",
        juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f), 1.f));

    juce::StringArray stringArray;
    for (int i = 0; i < 4; ++i) {
        juce::String str;
        str << (12 + i * 12);
        str << "db/Oct*";
        stringArray.add(str);
    }
    layout.add(std::make_unique<juce::AudioParameterChoice>("LowCut Slope"
        , "LowCut Slope", stringArray, 0));

    layout.add(std::make_unique<juce::AudioParameterChoice>("HighCut Slope"
        , "HighCut Slope", stringArray, 0));

    return layout;
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}
