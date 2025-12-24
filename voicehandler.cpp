//
// voicehandler.cpp
//

#include <circle/util.h>
#include <circle/logger.h>
#include "voicehandler.h"

static const char FromVoiceHandler[] = "VoiceHandler";

CVoiceHandler::CVoiceHandler(CMemorySystem *pMemorySystem, unsigned sampleRate)
: CMultiCoreSupport (pMemorySystem), 
  m_sampleRate(sampleRate),
  m_nFrames(0),
  m_activeCount(0),
  m_unison(false),
  m_unisonVoices(5),
  m_spread(0.015f),
  m_fine{ 0.5f, 0.5f }
{
	for (unsigned nCore = 0; nCore < CORES; nCore++)
	{
		m_CoreStatus[nCore] = CoreStatusInit;
		m_voicesBuffer[nCore] = 0.0f;
	}
}

CVoiceHandler::~CVoiceHandler()
{
	for (unsigned nCore = 1; nCore < CORES; nCore++)
	{
		assert (m_CoreStatus[nCore] == CoreStatusIdle);
		m_CoreStatus[nCore] = CoreStatusExit;

		while (m_CoreStatus[nCore] == CoreStatusExit)
		{
			// just wait
		}
	}
}

boolean CVoiceHandler::Initialize (void)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
			m_voices[i].InitVoice(m_sampleRate);	
	
	if (!CMultiCoreSupport::Initialize ())
	{
		return FALSE;
	}

	// wait for secondary cores to be ready
	for (unsigned nCore = 1; nCore < CORES; nCore++)
	{
		while (m_CoreStatus[nCore] != CoreStatusIdle)
		{
			// just wait
		}
	}

	return TRUE;
}


void CVoiceHandler::ResetVoiceHandler(void)
{
	m_unison = false;
	m_unisonVoices = 1;
	m_spread = 0.0f;
	m_fine[0] = 0.5f;	
	m_fine[1] = 0.5f;	
}







void CVoiceHandler::InitPatches(CSDLoad* pSD)
{
	m_pSD = pSD;
	m_currentPatch = 0;
	
	CPatch Patch(this);	// link patch to voicehandler
	
	const char* buf = m_pSD->GetPatchBuffer(m_currentPatch);
	unsigned len = m_pSD->GetPatchBufferSize(m_currentPatch);
	
	Patch.SetBuffer(buf, len);
	Patch.Parse();
}


void CVoiceHandler::SetPatch(unsigned patch)
{
	if (!m_pSD)
		return;
	
	ResetVoiceHandler();
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
			m_voices[i].InitVoice(m_sampleRate);	
	
	unsigned maxPatches = m_pSD->GetPatchCount();
	if (patch > maxPatches)
		return;
		
	CPatch Patch(this);	// link patch to voicehandler
	const char* buf = m_pSD->GetPatchBuffer(patch);
	unsigned len = m_pSD->GetPatchBufferSize(patch);
	
	Patch.SetBuffer(buf, len);
	Patch.Parse();
}



void CVoiceHandler::NoteOn(unsigned note, unsigned velocity)
{
	if (!m_unison) // POLY MODE
	{
		if (m_activeCount == VH_MAX_VOICES)
		{
			unsigned voiceKill = CVoiceHandler::FindQuietVoice();
			m_voices[voiceKill].NoteKill();
			m_voices[voiceKill].SetVolume(1.0f);
			m_voices[voiceKill].NoteOn(note, velocity);
			return;
		}
		
		for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
		{
			if (!m_voices[i].IsActive())
			{
				m_voices[i].NoteOn(note, velocity);
				return;
			}
		}
	}
	else // UNISON MODE
	{
		if (m_voices[0].IsActive())
		{
			for (unsigned i = 0; i < m_unisonVoices; ++i)
				m_voices[i].NoteSlowKill();
				
			m_voices[5].SetVolume(1.0f);
			m_voices[6].SetVolume(0.6f);			
			m_voices[7].SetVolume(0.4f);
			m_voices[8].SetVolume(0.2f);
			m_voices[9].SetVolume(0.1f);
			
			for (unsigned i = 5; i < (m_unisonVoices+5); ++i)
				m_voices[i].NoteOn(note, velocity);
		}
		else
		{
			for (unsigned i = 5; i < (m_unisonVoices+5); ++i)
				m_voices[i].NoteSlowKill();
				
			m_voices[0].SetVolume(1.0f);
			m_voices[1].SetVolume(0.6f);			
			m_voices[2].SetVolume(0.4f);
			m_voices[3].SetVolume(0.2f);
			m_voices[4].SetVolume(0.1f);
			
			for (unsigned i = 0; i < m_unisonVoices; ++i)
				m_voices[i].NoteOn(note, velocity);
		}
	}
}

unsigned CVoiceHandler::FindQuietVoice()
{
	float minVol = 1.0f;
	unsigned voice = 0;
	
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		float vol = m_voices[i].GetVolumeEnvAmplitude();
		if (vol < minVol)
		{
			minVol = vol;
			voice = i;
		}
	}
	return voice;
}


void CVoiceHandler::NoteOff(unsigned note)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		if (m_voices[i].HasKey(note))
			m_voices[i].NoteOff();
	}
}



//////////////////////////
//	Osc parameter   	//
//////////////////////////
void CVoiceHandler::SetCoarse(unsigned osc, float coarse)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetCoarse(osc, coarse);
	}
}

void CVoiceHandler::SetFine(unsigned osc, float fine)
{
	m_fine[osc] = fine;
	
	if (!m_unison)
	{
		for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
		{
			m_voices[i].SetFine(osc, m_fine[osc]);
		}
	}
	else
	{	
		for (unsigned i = 0; i < 10; ++i)
		{
			if (i<5)
			{
				m_voices[i].SetFine(0, m_fine[0] + m_spread * i);
				m_voices[i].SetFine(1, m_fine[1] - m_spread * i);
			}
			else
			{

				m_voices[i].SetFine(0, m_fine[0] + m_spread * (i-5));
				m_voices[i].SetFine(1, m_fine[1] - m_spread * (i-5));
			}
		}
	}
}

void CVoiceHandler::SetOscVol(unsigned osc, float vol)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetOscVol(osc, vol);
	}
}

void CVoiceHandler::SetWaveformSaw(unsigned osc, bool wave)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetWaveformSaw(osc, wave);
	}
}
void CVoiceHandler::SetWaveformSquare(unsigned osc, bool wave)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetWaveformSquare(osc, wave);
	}
}
void CVoiceHandler::SetWaveformTri(bool wave)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetWaveformTri(wave);
	}
}

void CVoiceHandler::SetPulseWidth(unsigned osc, float pw)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetPulseWidth(osc, pw);
	}
}
void CVoiceHandler::SetOsc2KbdTrk(bool kbd)
{
	unsigned val;
	val = (kbd) ? 1 : 0;
	
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetOsc2KbdTrk(val);
	}
}
void CVoiceHandler::SetOsc2LowFreq(bool low)
{
	unsigned val;
	val = (low) ? 1 : 0;
	
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetOsc2LowFreq(val);
	}
}

void CVoiceHandler::SetOsc1Sync(bool sync)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetOsc1Sync(sync);
	}
}

//////////////////////////
//	lfo parameter   	//
//////////////////////////
void CVoiceHandler::SetLfoFm(unsigned osc, bool fm)
{
	float val;
	val = (fm) ? 0.2f : 0.0f;
	
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetLfoFm(osc, val);
	}
}

void CVoiceHandler::SetLfoFilter(bool fil)
{
	float val;
	val = (fil) ? 0.5f : 0.0f;
	
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetLfoFilter(val);
	}
}

void CVoiceHandler::SetPulseWidthModulation(unsigned osc, bool pwm)
{
	float val;
	val = (pwm) ? 0.5f : 0.0f;
	
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetPulseWidthModulation(osc, val);
	}
}

void CVoiceHandler::SetLfoFreq(float freq)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetLfoFreq(freq);
	}
}

void CVoiceHandler::SetLfoWave(unsigned wave)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetLfoWave(wave);
	}
}

void CVoiceHandler::SetLfoMix(float mix)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetLfoMix(mix);
	}
}


//////////////////////////
//	Filter parameter	//
//////////////////////////
void CVoiceHandler::SetCutoff(float freq)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetCutoff(freq);
	}
}

void CVoiceHandler::SetResonance(float q)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetResonance(q);
	}
}

void CVoiceHandler::SetEnvAmt(float amt)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetFilterEnvAmt(amt);
	}
}

void CVoiceHandler::SetKbdTrk(float amt)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetFilterKbdTrk(amt);
	}
}



//////////////////
//	POLY MOD	//
//////////////////
void CVoiceHandler::SetPolyModFreq(bool polyFreq)
{
	unsigned val;
	val = (polyFreq) ? 1 : 0;
	
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetPolyModFreq(val);
	}
}

void CVoiceHandler::SetPolyModPw(bool polyPw)
{
	unsigned val;
	val = (polyPw) ? 1 : 0;
	
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetPolyModPw(val);
	}
}

void CVoiceHandler::SetPolyModFil(bool polyFil)
{
	unsigned val;
	val = (polyFil) ? 1 : 0;
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetPolyModFil(val);
	}
}

void CVoiceHandler::SetPolyModOsc2(float polyOsc2)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetPolyModOsc2(polyOsc2);
	}
}

void CVoiceHandler::SetPolyModFilEnv(float polyFilEnv)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetPolyModFilEnv(polyFilEnv);
	}
}

//////////////////
//	Envelopes	//
//////////////////
void CVoiceHandler::SetEnv(unsigned param, float value)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetEnv(param, value);
	}
}

void CVoiceHandler::SetNoiseVol(float n)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetNoiseVol(n);
	}
}


//////////////////////////
//	General Parameters	//
//////////////////////////
void CVoiceHandler::SetModWheel(float mod)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetModWheel(mod);
	}
}

void CVoiceHandler::SetGain(float gain)
{
	gain = (gain<0.0f) ? 0.0f : (gain>2.0f) ? 2.0f : gain; 

	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetGain(gain);
	}
}

void CVoiceHandler::SetGlideTime(float time)
{
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
	{
		m_voices[i].SetGlideTime(time);
	}
}

void CVoiceHandler::SetUnisonSpread(float spread)
{
	m_spread = spread * 0.3685f + 0.0315f;
	
	if (m_unison)
	{
		for (unsigned i = 0; i < 10; ++i)
		{
			if (i<5)
			{
				m_voices[i].SetFine(0, m_fine[0] + m_spread * i);
				m_voices[i].SetFine(1, m_fine[1] - m_spread * i);
			}
			else
			{

				m_voices[i].SetFine(0, m_fine[0] + m_spread * (i-5));
				m_voices[i].SetFine(1, m_fine[1] - m_spread * (i-5));
			}
		}
	}
}

void CVoiceHandler::SetUnisonVoices(unsigned v)
{
	m_unisonVoices = v;
	m_unisonVoices = (m_unisonVoices<1) ? 1 : (m_unisonVoices>5) ? 5 : m_unisonVoices; 
}

void CVoiceHandler::SetUnison(bool u)
{
	m_unison = u;
	
	if (m_unison)
	{
		for (unsigned i = 0; i < 10; ++i)
		{
			if (i<5)
			{
				m_voices[i].SetFine(0, m_fine[0] + m_spread * i);
				m_voices[i].SetFine(1, m_fine[1] - m_spread * i);
			}
			else
			{

				m_voices[i].SetFine(0, m_fine[0] + m_spread * (i-5));
				m_voices[i].SetFine(1, m_fine[1] - m_spread * (i-5));
			}
		}
	}
	else
	{
		for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
		{
			m_voices[i].SetVolume(1.0f);
			m_voices[i].SetFine(0, m_fine[0]);
			m_voices[i].SetFine(1, m_fine[1]);
		}
	}
}





void CVoiceHandler::Run (unsigned nCore)	// runs on secondary cores
{
	assert (1 <= nCore && nCore < CORES);
	unsigned nFirstVoice = nCore * VOICES_PER_CORE;
	unsigned nLastVoice  = nFirstVoice + VOICES_PER_CORE;

	while (1)
	{
		m_CoreStatus[nCore] = CoreStatusIdle;			// ready to be kicked
		while (m_CoreStatus[nCore] == CoreStatusIdle)
		{
			// just wait
		}

		if (m_CoreStatus[nCore] == CoreStatusExit)
		{
			m_CoreStatus[nCore] = CoreStatusUnknown;

			return;
		}

		assert (m_CoreStatus[nCore] == CoreStatusBusy);

		//// render voices on core 1-3
		m_voicesBuffer[nCore] = 0.0f;
		for (unsigned v = nFirstVoice; v < nLastVoice; ++v)
		{
			m_voicesBuffer[nCore] += RenderVoice(v);
		}
	}
}



float CVoiceHandler::RenderVoice(unsigned index)
{	
	m_activeCount = GetActiveVoiceCount();
	
	float sample = m_voices[index].Render();
	
	return sample;
}

float CVoiceHandler::RenderMixedVoices(void)
{	
	// kick secondary cores
	for (unsigned nCore = 1; nCore < CORES; nCore++)
	{
		assert (m_CoreStatus[nCore] == CoreStatusIdle);
		m_CoreStatus[nCore] = CoreStatusBusy;
	}
	

	// render voices on core 0
	m_voicesBuffer[0] = 0.0f;
	for (unsigned v = 0; v < VOICES_PER_CORE; ++v)
	{
		m_voicesBuffer[0] += RenderVoice(v);
	}
	
	// wait for secondary cores to complete their work
	for (unsigned nCore = 1; nCore < CORES; nCore++)
	{
		while (m_CoreStatus[nCore] != CoreStatusIdle)
		{
			// just wait
		}
	}
	
	float mixedVoices = m_voicesBuffer[0] + m_voicesBuffer[1] +
						m_voicesBuffer[2] + m_voicesBuffer[3];
						
	return mixedVoices;
}


unsigned CVoiceHandler::GetActiveVoiceCount() const
{
	unsigned count = 0;
	for (unsigned i = 0; i < VH_MAX_VOICES; ++i)
		if (m_voices[i].IsActive()) ++count;
	return count;
}

