//
// oscillator.h
//

#ifndef _OSCILLATOR_H_
#define _OSCILLATOR_H_

#include <circle/types.h>


class COscillator
{
public:
	COscillator();
	~COscillator() = default;

	void SetFrequency(unsigned key, float transpose, float fine, float lfo, float polyMod, unsigned sampleRate);
	void SetWaveformSaw(float wave){ m_waveformSaw = wave; }
	void SetWaveformSquare(float wave){ m_waveformSquare = wave; }
	void SetWaveformTri(float wave){ m_waveformTri = wave; }
	void SetPulseWidth(float pw);
	
	void SetOsc2KbdTrk(unsigned kbd){ m_osc2Kbd = kbd; }	
	void SetOsc2LowFreq(unsigned low){ m_osc2Low = low; }
	
	void SetGlideTime(float time);

	float GetSync(void) { return m_sync; }
	void SetSync(bool sync) { m_sync = sync; }
	
	void Reset();
	
	inline float PolyBlep(float t, float dt);
	inline float PolyBlamp(float t, float dt);
	
	float GetSaw(void);
	float GetSquare(void);
	float GetTri(void);
	float GetSample(void);

private:

	float m_waveformSaw;
	float m_waveformSquare;
	float m_waveformTri;

	float m_phase;       // 0..TABLE_SIZE
	float m_phaseInc;    // increment per sample in table indices
	float m_phaseOffset;
	
	float m_pulseWidth;
	float m_frequency;
	float m_currentFrequency;
	float m_targetFrequency;
	float m_note;
	
	float	m_glideTime;
	
	unsigned m_tableIndex;
	float 	 m_tableFrac;
	
	unsigned	m_osc2Kbd;
	unsigned    m_osc2Low;
	
	bool m_sync;
};

#endif
