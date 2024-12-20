#include "PluginProcessor.h"
#include "PluginEditor.h"

SawnthesizerAudioProcessor::SawnthesizerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
	apvts(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
	synth.addVoice(new SynthVoice());
	synth.addSound(new SynthSound());
}

SawnthesizerAudioProcessor::~SawnthesizerAudioProcessor()
{
}

const juce::String SawnthesizerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SawnthesizerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SawnthesizerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SawnthesizerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SawnthesizerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SawnthesizerAudioProcessor::getNumPrograms()
{
    return 1;
}

int SawnthesizerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SawnthesizerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SawnthesizerAudioProcessor::getProgramName (int index)
{
    return {};
}

void SawnthesizerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void SawnthesizerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	synth.setCurrentPlaybackSampleRate(sampleRate);
	for (int i = 0; i < synth.getNumVoices(); i++)
	{
		if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
		{
			voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
		}
	}
}

void SawnthesizerAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SawnthesizerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

// Process Block ==============================================================================
void SawnthesizerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    setParameters();

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    juce::dsp::AudioBlock<float> block{ buffer };
}
//==============================================================================

bool SawnthesizerAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SawnthesizerAudioProcessor::createEditor()
{
    return new SawnthesizerAudioProcessorEditor (*this);
}

void SawnthesizerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void SawnthesizerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SawnthesizerAudioProcessor();
}

int SawnthesizerAudioProcessor::getCurrentMidiNoteNumber() const
{
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            if (voice->isVoiceActive())
                return voice->getCurrentMidiNoteNumber();
        }
    }
    return -1;
}

juce::AudioProcessorValueTreeState::ParameterLayout SawnthesizerAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("SYNTH_GAIN", "Synth Volume", juce::NormalisableRange<float> { -40.0f, 6.0f, 0.1f }, 0.1f, "dB"));

    //params.push_back(std::make_unique<juce::AudioParameterChoice>("FILTERTYPE", "Filter Type", juce::StringArray{ "Low Pass", "Band Pass", "High Pass" }, 0));
    //params.push_back(std::make_unique<juce::AudioParameterFloat>("FILTERCUTOFF", "Filter Cutoff", juce::NormalisableRange<float> { 20.0f, 20000.0f, 0.1f, 0.6f }, 20000.0f, "Hz"));
    //params.push_back(std::make_unique<juce::AudioParameterFloat>("FILTERRESONANCE", "Filter Resonance", juce::NormalisableRange<float> { 0.1f, 2.0f, 0.1f }, 0.1f, ""));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("ATTACK", "Attack", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DECAY", "Decay", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("SUSTAIN", "Sustain", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RELEASE", "Release", juce::NormalisableRange<float> { 0.1f, 3.0f, 0.1f }, 0.4f));

    //params.push_back(std::make_unique<juce::AudioParameterFloat>("FILTERADSRDEPTH", "Filter ADSR Depth", juce::NormalisableRange<float> { 0.0f, 10000.0f, 0.1f, 0.3f }, 10000.0f, ""));
    //params.push_back(std::make_unique<juce::AudioParameterFloat>("FILTERATTACK", "Filter Attack", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.01f));
    //params.push_back(std::make_unique<juce::AudioParameterFloat>("FILTERDECAY", "Filter Decay", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 0.1f));
    //params.push_back(std::make_unique<juce::AudioParameterFloat>("FILTERSUSTAIN", "Filter Sustain", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 1.0f));
    //params.push_back(std::make_unique<juce::AudioParameterFloat>("FILTERRELEASE", "Filter Release", juce::NormalisableRange<float> { 0.0f, 3.0f, 0.1f }, 0.1f));

    return { params.begin(), params.end() };
}

void SawnthesizerAudioProcessor::setParameters()
{
    juce::ADSR::Parameters adsrParameters;
    adsrParameters.attack = *apvts.getRawParameterValue("ATTACK");
    adsrParameters.decay = *apvts.getRawParameterValue("DECAY");
    adsrParameters.sustain = *apvts.getRawParameterValue("SUSTAIN");
    adsrParameters.release = *apvts.getRawParameterValue("RELEASE");
	float gain = *apvts.getRawParameterValue("SYNTH_GAIN");
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->setParameters(adsrParameters,gain);
        }
    }
}