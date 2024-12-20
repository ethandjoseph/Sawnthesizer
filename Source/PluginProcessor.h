#pragma once
#include <JuceHeader.h>
#include "SynthVoice.h"
#include "SynthSound.h"

class SawnthesizerAudioProcessor  : public juce::AudioProcessor
{
public:
    SawnthesizerAudioProcessor();
    ~SawnthesizerAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    int getCurrentMidiNoteNumber() const;

    juce::AudioProcessorValueTreeState apvts;

private:
	juce::Synthesiser synth;
    int currentMidiNoteNumber{ -1 };
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void setParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SawnthesizerAudioProcessor)
};