//
// lfo.cpp
//

#include "lfo.h"

CLFO::CLFO(unsigned sampleRate)
: m_sampleRate(sampleRate),
  m_waveSaw(0.0f),
  m_waveSquare(0.0f),
  m_waveTri(1.0f),
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

    return (square * m_waveSquare - saw * m_waveSaw + tri * m_waveTri);
}
