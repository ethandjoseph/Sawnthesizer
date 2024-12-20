#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class SawnthesizerAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    SawnthesizerAudioProcessorEditor (SawnthesizerAudioProcessor&);
    ~SawnthesizerAudioProcessorEditor() override;

	void timerCallback() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider gainSlider; // slider itself needs to outlive the attachment - so attachment needs to below (destruction happens upwards)
	juce::Slider attackSlider;
	juce::Slider decaySlider;
	juce::Slider sustainSlider;
	juce::Slider releaseSlider;
    juce::Label gainLabel;
	juce::Label attackLabel;
	juce::Label decayLabel;
	juce::Label sustainLabel;
	juce::Label releaseLabel;
	juce::Label midiNoteLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decaySliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseSliderAttachment;

    SawnthesizerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SawnthesizerAudioProcessorEditor)
};
