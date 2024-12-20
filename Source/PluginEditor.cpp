#include "PluginProcessor.h"
#include "PluginEditor.h"

SawnthesizerAudioProcessorEditor::SawnthesizerAudioProcessorEditor (SawnthesizerAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
    gainSlider.setSliderStyle(juce::Slider::LinearVertical);
	attackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
	decaySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
	sustainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
	releaseSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);

    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
	attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
	decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
	sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
	releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);

    gainLabel.setText("Volume", juce::dontSendNotification);
	attackLabel.setText("Attack", juce::dontSendNotification);
	decayLabel.setText("Decay", juce::dontSendNotification);
	sustainLabel.setText("Sustain", juce::dontSendNotification);
	releaseLabel.setText("Release", juce::dontSendNotification);
	midiNoteLabel.setText("MIDI Note: None", juce::dontSendNotification);

    gainLabel.attachToComponent(&gainSlider, false);
	attackLabel.attachToComponent(&attackSlider, false);
	decayLabel.attachToComponent(&decaySlider, false);
	sustainLabel.attachToComponent(&sustainSlider, false);
	releaseLabel.attachToComponent(&releaseSlider, false);

	addAndMakeVisible(gainSlider);
	addAndMakeVisible(attackSlider);
	addAndMakeVisible(decaySlider);
	addAndMakeVisible(sustainSlider);
	addAndMakeVisible(releaseSlider);

    addAndMakeVisible(gainLabel);
	addAndMakeVisible(attackLabel);
	addAndMakeVisible(decayLabel);
	addAndMakeVisible(sustainLabel);
	addAndMakeVisible(releaseLabel);
	addAndMakeVisible(midiNoteLabel);

    gainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SYNTH_GAIN", gainSlider);
	attackSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "ATTACK", attackSlider);
	decaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DECAY", decaySlider);
	sustainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SUSTAIN", sustainSlider);
	releaseSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "RELEASE", releaseSlider);
	
	startTimerHz(10);

    setSize (800, 200);
}

SawnthesizerAudioProcessorEditor::~SawnthesizerAudioProcessorEditor()
{
}

void SawnthesizerAudioProcessorEditor::timerCallback()
{
	auto midiNoteNumber = audioProcessor.getCurrentMidiNoteNumber();
	if (midiNoteNumber >= 0)
	{
		auto noteName = juce::MidiMessage::getMidiNoteName(midiNoteNumber, true, true, 3);
		midiNoteLabel.setText("MIDI Note: " + noteName, juce::dontSendNotification);
	}
	else
	{
		midiNoteLabel.setText("MIDI Note: None", juce::dontSendNotification);
	}
}

void SawnthesizerAudioProcessorEditor::paint (juce::Graphics& g)
{
	g.fillAll(juce::Colours::darkviolet);
}

void SawnthesizerAudioProcessorEditor::resized()
{
	auto area = getLocalBounds().reduced(40);
	int sliderWidth = area.getWidth() / 6;
	int sliderHeight = area.getHeight();
	
	midiNoteLabel.setBounds(area.removeFromLeft(sliderWidth).removeFromTop(sliderHeight));
	attackSlider.setBounds(area.removeFromLeft(sliderWidth).removeFromTop(sliderHeight));
	decaySlider.setBounds(area.removeFromLeft(sliderWidth).removeFromTop(sliderHeight));
	sustainSlider.setBounds(area.removeFromLeft(sliderWidth).removeFromTop(sliderHeight));
	releaseSlider.setBounds(area.removeFromLeft(sliderWidth).removeFromTop(sliderHeight));
	gainSlider.setBounds(area.removeFromLeft(sliderWidth).removeFromTop(sliderHeight));
}