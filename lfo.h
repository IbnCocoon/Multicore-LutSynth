//
// lfo.h
//

#ifndef LFO_H
#define LFO_H

#include <circle/util.h>


class CLFO
{
public:

    CLFO(unsigned sampleRate);

    void Init(float sampleRate);
    
	void SetWaveSaw(float wave) { m_waveSaw = wave; }
	void SetWaveSquare(float wave) { m_waveSquare = wave; }
	void SetWaveTri(float wave) { m_waveTri = wave; }
	
    void SetFrequency(float freqHz); 
    void Reset(void);

    float GetSample(void);

private:

    float     m_sampleRate;
	float	  m_waveSaw;
	float	  m_waveSquare;
	float	  m_waveTri;
    float     m_frequency;
    float     m_phase;
    float     m_phaseInc;
};

#endif
