//
// voice.h
//

#ifndef _VOICE_H_
#define _VOICE_H_

#include <circle/types.h>

#include "oscillator.h"
#include "envelope.h"
#include "moogfilter.h"
#include "lfo.h"
#include "noise.h"

class CVoice
{
public:
	CVoice(unsigned sampleRate = 48000);
	~CVoice() = default;
	
	void InitVoice(unsigned sampleRate);

	void NoteOn(unsigned note, unsigned velocity);
	void NoteOff();
	void NoteSlowKill();
	void NoteKill();
	
	// Oscillator parameters
	void SetCoarse(unsigned osc, float coarse);
	void SetFine(unsigned osc, float fine){ m_fine[osc] = fine; }
	void SetWaveformSaw(unsigned osc, bool wave);
	void SetWaveformSquare(unsigned osc, bool wave);
	void SetWaveformTri(bool wave);
	void SetPulseWidth(unsigned osc, float pw){ m_pulseWidth[osc] = pw; }
	void SetOscVol(unsigned osc, float vol){ m_oscVol[osc] = vol; }	
	void SetOsc2KbdTrk(unsigned kbd){ m_osc2Kbd = kbd; }	
	void SetOsc2LowFreq(unsigned low){ m_osc2Low = low; }
	void SetOsc1Sync(bool sync){ m_osc1Sync = sync; }
	
	// Filter Parameters
	void SetCutoff(float freq);
	void SetResonance(float res);
	void SetFilterEnvAmt(float amt){ m_filEnvAmt = amt; }
	void SetFilterKbdTrk(float amt){ m_filKbdTrkAmt = amt; }
	
	// Lfo Parameters
	void SetLfoFreq(float freq);
	void SetLfoWave(unsigned wave);
	void SetLfoFm(unsigned osc, float fm){ m_lfoFm[osc] = fm; }
	void SetPulseWidthModulation(unsigned osc, float pwm){ m_lfoPwm[osc] = pwm; }
	void SetLfoFilter(float fil){ m_lfoFilter = fil; }
	void SetLfoMix(float mix){ m_lfoMix = mix; }
	
	// Polymod Parameters
	void SetPolyModFreq(unsigned polyFreq){ m_polyModFreq = polyFreq; }
	void SetPolyModPw(unsigned polyPw){ m_polyModPw = polyPw; }
	void SetPolyModFil(unsigned polyFil){ m_polyModFil = polyFil; }
	void SetPolyModOsc2(float polyOsc2){ m_polyModOsc2 = polyOsc2; }
	void SetPolyModFilEnv(float polyFilEnv){ m_polyModFilEnv = polyFilEnv; }
	
	// Envelope Parameters
	void SetEnv(unsigned param, float value);
	
	// Noise Parameters
	void SetNoiseVol(float n){ m_noiseVol = n; }
	
	// General Parameters
	void SetModWheel(float mod){ m_modWheel = mod; }
	void SetVolume(float vol){ m_voiceVolume = vol; }
	void SetGain(float gain);
	void SetGlideTime(float time);
		
	bool IsActive() const;
	bool HasKey(unsigned note) const;
	
	float GetVolumeEnvAmplitude(void) { return m_envVol; }
	unsigned GetNote(void) { return m_note; }

	float Render(void);

private:
	float 		m_sampleRate;
	bool 		m_active;
	unsigned 	m_note;
	unsigned 	m_velocity;
	
	// Objects
	COscillator m_osc[2];
	CEnvelope 	m_envelope[2];
	CLFO	 	m_lfo;
	CNoise	 	m_noise;
	CMoogFilter	m_moogFilter;
	
	// Oscillator
	float 		m_coarse[2];
	float 		m_fine[2];
	bool 		m_waveformSaw[2];
	bool 		m_waveformSquare[2];
	bool 		m_waveformTri;
	float 		m_pulseWidth[2];
	float 		m_oscVol[2];
	unsigned	m_osc2Kbd;
	unsigned    m_osc2Low;
	bool	    m_osc1Sync;
	
	// Filter
	float 		m_filCutoff;
	float 		m_filRes;
	float 		m_filEnvAmt;
	float 		m_filKbdTrk;
	float		m_filKbdTrkAmt;
	
	// Lfo
	float 		m_lfoFreq;
	unsigned	m_lfoWave;
	float		m_lfoMix;
	float	 	m_lfoFm[2];
	float 		m_lfoPwm[2];
	float 		m_lfoFilter;

	// Polymod
	unsigned 	m_polyModFreq;
	unsigned 	m_polyModPw;
	unsigned 	m_polyModFil;
	float 		m_polyModOsc2;
	float 		m_polyModFilEnv;
	
	// Envelope
	float m_envVol;
	
	// Noise
	float m_noiseVol;
	
	// General
	float m_modWheel;
	float m_voiceVolume;
	float m_gain;
};

#endif
