//
// voice.cpp
//

#include "voice.h"

#define VOL_ATTACK  1
#define VOL_DECAY	2
#define VOL_SUSTAIN 3
#define VOL_RELEASE 4
#define FIL_ATTACK  5
#define FIL_DECAY	6
#define FIL_SUSTAIN 7
#define FIL_RELEASE 8
#define OSC_COARSE	0


CVoice::CVoice(unsigned sampleRate)
: m_sampleRate(sampleRate),
  m_active(false),
  m_note(0),
  m_velocity(0),
  m_envelope{
				CEnvelope(sampleRate), // Envelope 0
				CEnvelope(sampleRate)  // Envelope 1
			},
  m_lfo(sampleRate),
  m_moogFilter(sampleRate),
  
  // Oscillator
  m_coarse{ 0, 0 }, 
  m_fine{ 0.5f, 0.5f },
  m_waveformSaw{ true, true },
  m_waveformSquare{ false, false },
  m_waveformTri(false),
  m_pulseWidth{ 0.5f, 0.5f },
  m_oscVol{ 1.0f, 0.0f },
  m_osc2Kbd(1),
  m_osc2Low(0),
  
  // Filter
  m_filCutoff(5000.0f),
  m_filRes(0.0f),
  m_filEnvAmt(0),
  m_filKbdTrk(0),
  m_filKbdTrkAmt(0),
  
  // Lfo
  m_lfoFreq(0.1f),
  m_lfoWaveSaw(false),
  m_lfoWaveSquare(false),
  m_lfoWaveTri(true),
  m_lfoMix(1.0f),
  m_lfoFm{ false , false },
  m_lfoPwm{ 0, 0 },
  m_lfoFilter(0),

  // Polymod
  m_polyModFreq(0),
  m_polyModPw(0),
  m_polyModFil(0),
  m_polyModOsc2(0),
  m_polyModFilEnv(0),
  
  // Envelope
  m_envVol(0),
  
  // Noise
  m_noiseVol(0),
  
  // General
  m_modWheel(0),
  m_voiceVolume(1.0f),
  m_gain(0.1f)
{
}


void CVoice::InitVoice(unsigned sampleRate)
{
	m_active = false;
	m_note = 0;
	m_velocity = 0;
	
	m_coarse[0] = 0;
	m_coarse[1] = 0;
	m_fine[0] = 0.5f;
	m_fine[1] = 0.5f;
	m_waveformSaw[0] = true;
	m_waveformSaw[1] = true;
	m_waveformSquare[0] = false;
	m_waveformSquare[1] = false;
	m_waveformTri = false;
	m_pulseWidth[0] = 0.5f;
	m_pulseWidth[1] = 0.5f;
	m_oscVol[0] = 1.0f;
	m_oscVol[1] = 1.0f;
	m_osc1Sync = false;
	m_osc2Kbd = 1;
	m_osc2Low = 0;
	m_osc[0].Reset();
	m_osc[1].Reset();
	m_osc[0].SetFrequency(0, m_coarse[0], m_fine[0], 0.0f, 0.0f, m_sampleRate);
	m_osc[1].SetFrequency(0, m_coarse[1], m_fine[1], 0.0f, 0.0f, m_sampleRate);
	
	m_filCutoff = 5000.0f;
	m_filRes = 0.0f;
	m_filEnvAmt = 0.0f;
	m_filKbdTrk = 0.0f;
	m_filKbdTrkAmt = 0.0f;
	
	m_lfoFreq = 0.5f;
	m_lfoWaveSaw = false;
	m_lfoWaveSquare = false;
	m_lfoWaveTri = true;
	m_lfoMix = 1.0f;
	m_lfoFm[0] = 0;
	m_lfoFm[1] = 0;
	m_lfoPwm[0] = 0;
	m_lfoPwm[1] = 0;
	m_lfoFilter = 0;

	m_polyModFreq = 0;
	m_polyModPw = 0;
	m_polyModFil = 0;
	m_polyModOsc2 = 0.0f;
	m_polyModFilEnv = 0.0f;
	
	m_noiseVol = 0.0f;
	m_modWheel = 0.0f;
	m_voiceVolume = 1.0f;
	m_gain = 1.0f;
	
	m_envelope[0].SetAttack(100.0f);
	m_envelope[0].SetDecay(100.0f);
	m_envelope[0].SetSustain(1.0f);
	m_envelope[0].SetRelease(150.0f);
	m_envelope[1].SetAttack(100.0f);
	m_envelope[1].SetDecay(100.0f);
	m_envelope[1].SetSustain(1.0f);
	m_envelope[1].SetRelease(150.0f);
	
	m_moogFilter.Reset();
	
	m_lfo.Init(m_sampleRate);
}

void CVoice::SetCutoff(float freq)
{
	m_filCutoff = freq;
	m_moogFilter.SetCutoff(m_filCutoff);
}

void CVoice::SetResonance(float res)
{
	m_filRes = res;
	m_moogFilter.SetResonance(m_filRes);
}

void CVoice::SetGain(float gain)
{
	m_gain = gain * 0.1f;
}

void CVoice::SetGlideTime(float time)
{
	m_osc[0].SetGlideTime(time);
	m_osc[1].SetGlideTime(time);
}

void CVoice::SetLfoFreq(float freq) { m_lfo.SetFrequency(freq); }
void CVoice::SetLfoWaveSaw(bool wave) { m_lfoWaveSaw = wave; }
void CVoice::SetLfoWaveSquare(bool wave) { m_lfoWaveSquare = wave; }
void CVoice::SetLfoWaveTri(bool wave) { m_lfoWaveTri = wave; }

void CVoice::SetEnv(unsigned param, float value)
{
	switch(param)
	{
		case VOL_ATTACK:  m_envelope[0].SetAttack(value);  break;
		case VOL_DECAY:	  m_envelope[0].SetDecay(value);   break;
		case VOL_SUSTAIN: m_envelope[0].SetSustain(value); break;
		case VOL_RELEASE: m_envelope[0].SetRelease(value); break;
		case FIL_ATTACK:  m_envelope[1].SetAttack(value);  break;
		case FIL_DECAY:	  m_envelope[1].SetDecay(value);   break;
		case FIL_SUSTAIN: m_envelope[1].SetSustain(value); break;
		case FIL_RELEASE: m_envelope[1].SetRelease(value); break;
		default: break;
	}
}

void CVoice::SetCoarse(unsigned osc, float coarse)
{
	if (!osc) 							
		m_coarse[osc] = (signed)coarse; // osc 1 - only stepped knob
	else
	{
		if (m_osc2Kbd) 
			m_coarse[osc] = (signed)coarse; // osc 2 - stepped if kbd track is active
		else
			m_coarse[osc] = coarse;			// osc 2 - continous knob if kbd is inactive
	}
}

void CVoice::SetWaveformSaw(unsigned osc, bool wave) { m_waveformSaw[osc] = wave; }
void CVoice::SetWaveformSquare(unsigned osc, bool wave) { m_waveformSquare[osc] = wave; }
void CVoice::SetWaveformTri(bool wave) { m_waveformTri = wave; }

void CVoice::NoteOn(unsigned note, unsigned velocity)
{
	m_note = note;
	m_velocity = velocity;
	m_osc[0].Reset();
	m_osc[1].Reset();
	m_envelope[0].NoteOn();
	m_envelope[1].NoteOn();
	
	m_active = true;
}

void CVoice::NoteOff()
{
	m_envelope[0].NoteOff();
	m_envelope[1].NoteOff();
}

void CVoice::NoteSlowKill()
{
	m_envelope[0].NoteSlowKill();
	m_envelope[1].NoteSlowKill();
}

void CVoice::NoteKill()
{
	m_envelope[0].NoteKill();
	m_envelope[1].NoteKill();
	m_active = false;
}

bool CVoice::IsActive() const
{
	return m_active;
}

bool CVoice::HasKey(unsigned note) const
{
	return m_active && (m_note == note);
}


float CVoice::Render(void)
{

	// Noise Processing //
	float noise = m_noise.GetSample();
	
	////////////////////
	// LFO Processing //
	////////////////////
	m_lfo.SetWaveSaw(m_lfoWaveSaw);
	m_lfo.SetWaveSquare(m_lfoWaveSquare);
	m_lfo.SetWaveTri(m_lfoWaveTri);
	float lfo = m_lfo.GetSample();		
	float lfoMix = lfo * m_lfoMix + noise * (1.0f - m_lfoMix);
	lfoMix *= m_modWheel;
	
	float lfoFm1 = lfoMix * m_lfoFm[0];	// lfo to osc 1 freq modulation
	float lfoFm2 = lfoMix * m_lfoFm[1];	// lfo to osc 1 freq modulation
	float lfoFil = lfoMix * m_lfoFilter;	// lfo to filter cutoff modulation
	float lfoPwm0 = lfoMix * m_lfoPwm[0];	// lfo to osc 1 pwm
	float lfoPwm1 = lfoMix * m_lfoPwm[1];  // lfo to osc 2 pwm
	
	
	//////////////////////
	// OSC 2 Processing //
	//////////////////////
	m_osc[1].SetOsc2KbdTrk(m_osc2Kbd);
	m_osc[1].SetOsc2LowFreq(m_osc2Low);
	m_osc[1].SetWaveformSaw(m_waveformSaw[1]);
	m_osc[1].SetWaveformSquare(m_waveformSquare[1]);
	m_osc[1].SetWaveformTri(m_waveformTri);
	m_osc[1].SetPulseWidth(m_pulseWidth[1] + lfoPwm1);
	m_osc[1].SetFrequency(m_note, m_coarse[1], m_fine[1], lfoFm1, 0.0f, m_sampleRate);
	float osc2 = m_osc[1].GetSample();
	
	
	////////////////////////////////
	// FILTER ENVELOPE Processing //
	////////////////////////////////
	float envFil = m_envelope[1].Process();			
	
	
	/////////////////////////
	// POLY-MOD Processing //
	/////////////////////////
	float polyModFilEnvSource = m_polyModFilEnv * envFil; 
	float polyModOsc2Source   = m_polyModOsc2 * osc2;
	float polyModOut = polyModFilEnvSource + polyModOsc2Source;
	
	//////////////////////
	// OSC 1 Processing //
	//////////////////////
	if (m_osc1Sync && m_osc[1].GetSync() )
	{
		m_osc[1].SetSync(false);
		m_osc[0].Reset();
	}
	m_osc[0].SetWaveformSaw(m_waveformSaw[0]);
	m_osc[0].SetWaveformSquare(m_waveformSquare[0]);
	m_osc[0].SetPulseWidth(m_pulseWidth[0] + lfoPwm0 + m_polyModPw * polyModOut);
	m_osc[0].SetFrequency(m_note, m_coarse[0], m_fine[0], lfoFm2, m_polyModFreq * polyModOut, m_sampleRate);
	float osc1 = m_osc[0].GetSample();				


	/////////////////////
	// MIX  Processing //
	/////////////////////
	float mix = osc1 * m_oscVol[0] + osc2 * m_oscVol[1] + noise * m_noiseVol; // mix oscillators & noise
	
	////////////////////////
	// FILTER  Processing //
	////////////////////////
	m_filKbdTrk = m_note * 0.00787f * m_filKbdTrkAmt;	// midi note 127 -> 1.0f
	float cutoffMod = envFil * m_filEnvAmt + polyModOut * m_polyModFil + lfoFil + m_filKbdTrk;
	cutoffMod *= m_sampleRate * 0.5f;
	m_moogFilter.SetCutoff(m_filCutoff + cutoffMod);
	float sample = m_moogFilter.Process(mix);

	////////////////////////////////
	// VOLUME ENVELOPE Processing //
	////////////////////////////////
	m_envVol = m_envelope[0].Process();				// get volume envelope sample
	sample *= m_envVol * m_voiceVolume * m_gain;	// apply vca

	if(!m_envelope[0].IsActive())
		m_active = false;
		
	return sample;
}



