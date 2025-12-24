//
// lfo.h
//

#ifndef LFO_H
#define LFO_H

#include <circle/util.h>


class CLFO
{
public:
    enum Waveform
    {

    };

    CLFO(unsigned sampleRate);

    void Init(float sampleRate);
	void SetWaveform(unsigned wave);
    void SetFrequency(float freqHz); 
    void Reset(void);

    float GetSample(void);

private:
	enum class WaveformMode{ Saw, Square, Tri, SawSquare, SawTri, SquareTri, SawSquareTri };
	WaveformMode m_waveform { WaveformMode::Tri };
    float     m_sampleRate;
    float     m_frequency;
    float     m_phase;
    float     m_phaseInc;
};

#endif
