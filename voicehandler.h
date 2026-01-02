//
// voicehandler.h
//
#ifndef _VOICEHANDLER_H_
#define _VOICEHANDLER_H_

#include <circle/types.h>
#include <circle/string.h>
#include <circle/multicore.h>
#include <circle/memory.h>

#include "voice.h"
#include "patch.h"
#include "sdload.h"

#define VH_MAX_VOICES   24
#define VOICES_PER_CORE 6

enum TCoreStatus
{
	CoreStatusInit,
	CoreStatusIdle,
	CoreStatusBusy,
	CoreStatusExit,
	CoreStatusUnknown
};



class CVoiceHandler
: public CMultiCoreSupport
{
public:
	// sampleRate needed for voices when NoteOn occurs
	CVoiceHandler(CMemorySystem *pMemorySystem, unsigned sampleRate);
	~CVoiceHandler();

	// MIDI events: these are the public API your existing MidiHandler/SerialMIDI can call
	void NoteOn(unsigned note, unsigned velocity);
	void NoteOff(unsigned note);
	unsigned FindQuietVoice(void);
	
	void SetPatch(unsigned patch);
	
	
	// Oscillator parameters
	void SetCoarse(unsigned osc, float coarse);
	void SetFine(unsigned osc, float fine);
	void SetWaveformSaw(unsigned osc, bool wave);
	void SetWaveformSquare(unsigned osc, bool wave);
	void SetWaveformTri(bool wave);
	void SetPulseWidth(unsigned osc, float pw);
	void SetOscVol(unsigned osc, float fine);
	void SetOsc2KbdTrk(bool kbd);
	void SetOsc2LowFreq(bool low);
	void SetOsc1Sync(bool sync);
	
	// Filter Parameters
	void SetCutoff(float freq);
	void SetResonance(float res);
	void SetEnvAmt(float amt);
	void SetKbdTrk(float amt);
	
	// Lfo Parameters
	void SetLfoFreq(float freq);
	void SetLfoWaveSaw(bool wave);
	void SetLfoWaveSquare(bool wave);
	void SetLfoWaveTri(bool wave);
	void SetLfoFm(unsigned osc, bool fm);
	void SetPulseWidthModulation(unsigned osc, bool pwm);
	void SetLfoFilter(bool fil);
	void SetLfoMix(float mix);
	
	// PolyMod Parameters
	void SetPolyModFreq(bool polyFreq);
	void SetPolyModPw(bool polyPw);
	void SetPolyModFil(bool polyFil);
	void SetPolyModOsc2(float polyOsc2);
	void SetPolyModFilEnv(float polyFilEnv);
	
	// Envelope Parameters
	void SetEnv(unsigned param, float value);
	
	// Noise Parameters
	void SetNoiseVol(float n);
	
	// General Parameters
	void SetModWheel(float mod);
	void SetUnisonVoices(unsigned v);
	void SetUnison(bool u);
	void SetUnisonSpread(float spread);
	void SetGlideTime(float time);
	void SetGain(float gain);

	// Voice access for mixer:
	// RenderVoice writes the specified voice's mono samples into pBuffer
	float RenderVoice(unsigned index);
	float RenderMixedVoices(void);
	
	// Return number of voices
	unsigned GetVoiceCount() const { return VH_MAX_VOICES; }

	// Return number of active voices (helper)
	unsigned GetActiveVoiceCount() const;
	
	bool IsVoiceActive(unsigned index) const { return m_voices[index].IsActive(); }

	boolean Initialize (void);
	void Run (unsigned nCore);			// secondary core entry
	
	void InitPatches(CSDLoad* pSD);
	
	void ResetVoiceHandler(void);
	
private:
	CVoice 			 m_voices[VH_MAX_VOICES];
	unsigned 		 m_sampleRate;
	CPatch			 m_patch;
	CSDLoad*		 m_pSD;
	
	unsigned		m_nFrames;
	
	unsigned		m_activeCount;
	bool			m_unison;
	unsigned		m_unisonVoices;
	float			m_spread;
	float			m_fine[2];
	
	unsigned		m_currentPatch;
	
	volatile TCoreStatus m_CoreStatus[CORES];
	volatile float m_voicesBuffer[CORES];
};

#endif
