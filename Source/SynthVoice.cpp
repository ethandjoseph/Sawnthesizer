#include "SynthVoice.h"

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
	return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
	osc.setFrequency(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
    adsr.noteOn();
    currentMidiNoteNumber = midiNoteNumber;
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
	adsr.noteOff();
    if (!allowTailOff || !adsr.isActive())
        clearCurrentNote();
        currentMidiNoteNumber = -1;
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    gain.reset();
    adsr.reset();

    adsr.setSampleRate(sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = outputChannels;

    osc.prepare(spec);
    gain.prepare(spec);

    gain.setGainLinear(0.01f);

	isPrepared = true;
}

void SynthVoice::setParameters(juce::ADSR::Parameters adsrParameters, float synth_gain)
{
    adsr.setParameters(adsrParameters);
	gain.setGainDecibels(synth_gain);
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
	jassert(isPrepared);

    if (!isVoiceActive())
        return;

	synthBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
    synthBuffer.clear();

    for (int ch = 0; ch < synthBuffer.getNumChannels(); ++ch)
    {
        auto* buffer = synthBuffer.getWritePointer(ch, 0);

        for (int s = 0; s < synthBuffer.getNumSamples(); ++s)
        {
            buffer[s] = osc.processSample(buffer[s]);
        }
    }

    juce::dsp::AudioBlock<float> audioBlock{ synthBuffer };
    gain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    adsr.applyEnvelopeToBuffer(synthBuffer, 0, synthBuffer.getNumSamples());

    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
    {
        outputBuffer.addFrom(channel, startSample, synthBuffer, channel, 0, numSamples);

        if (!adsr.isActive())
            clearCurrentNote();
    }
}

int SynthVoice::getCurrentMidiNoteNumber() const
{
	return currentMidiNoteNumber;
}