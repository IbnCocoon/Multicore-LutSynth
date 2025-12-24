//
// lfo.cpp
//

#include "lfo.h"

CLFO::CLFO(unsigned sampleRate)
: m_sampleRate(sampleRate),
  m_frequency(1.0f),
  m_phase(0.0f),
  m_phaseInc(0.0f)
{
}

void CLFO::Init(float sampleRate)
{
    m_sampleRate = sampleRate;
    SetFrequency(m_frequency);
    m_phase = 0.0f;
}

void CLFO::SetWaveform(unsigned wave)
{
	switch(wave)
	{
		case 0: m_waveform = WaveformMode::Saw; break;
		case 1: m_waveform = WaveformMode::Square; break;
		case 2: m_waveform = WaveformMode::Tri; break;
		case 3: m_waveform = WaveformMode::SawSquare; break;
		case 4: m_waveform = WaveformMode::SawTri; break;
		case 5: m_waveform = WaveformMode::SquareTri; break;
		case 6: m_waveform = WaveformMode::SawSquareTri; break;
		default:m_waveform = WaveformMode::Saw; break;
	}
}

void CLFO::SetFrequency(float freqHz)
{
    m_frequency = freqHz;

    m_frequency = (m_frequency<0.01f) ? 0.01f : (m_frequency>30.0f) ? 30.0f : m_frequency; 
    m_phaseInc = (m_frequency / m_sampleRate);   // normalized [0 → 1)
}

void CLFO::Reset(void)
{
    m_phase = 0.0f;
}

// ------------------------------------------------------------
// Generate one LFO sample in range -1 → +1
// ------------------------------------------------------------
float CLFO::GetSample(void)
{
    float saw = 0.0f;
    float square = 0.0f;
    float tri = 0.0f;
    float sample = 0.0f;

	// triangle //
	if (m_phase < 0.25f)
		tri = 4.0f * m_phase;
	else if (m_phase < 0.75f)
		tri = 2.0f - 4.0f * m_phase;
	else
		tri = -4.0f + 4.0f * m_phase;

	// square //
    square = (m_phase < 0.5f) ? 1.0f : -1.0f;

	// saw //
    saw = 2.0f * m_phase - 1.0f;

    m_phase += m_phaseInc;
    if (m_phase >= 1.0f)
        m_phase -= 1.0f;

    switch (m_waveform)
    {
		case WaveformMode::Saw: sample = saw; break;
		case WaveformMode::Square: sample = square; break;
		case WaveformMode::Tri: sample = tri; break;
		case WaveformMode::SawSquare: sample = (saw + square) * 0.5f; break;
		case WaveformMode::SawTri: sample = (saw + tri) * 0.5f; break;
		case WaveformMode::SquareTri: sample = (square + tri) * 0.5f; break;
		case WaveformMode::SawSquareTri: sample = (saw + square + tri) * 0.333f; break;
    }
    
    return sample;
}
